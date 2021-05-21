#include "CEDXDevice.h"

#include <dxgi1_5.h>

#include "../../../Utilities/CEStringUtilities.h"
#include "../../../Memory/CEMemory.h"
#include "../../../Core/Log/CELog.h"

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;

static const char* DeviceRemovedDumpFile = "D3D12DeviceRemovedDump.txt";

void DeviceRemovedHandler(CEDXDevice* Device) {
}

CEDXDevice::CEDXDevice(bool enableDebugLayer, bool enableGPUValidation, bool EnableDRED) :
	Factory(nullptr),
	Adapter(nullptr),
	Device(nullptr),
	DXRDevice(nullptr),
	EnableDebugLayer(enableDebugLayer),
	EnableGPUValidation(enableGPUValidation),
	EnableDRED(EnableDRED) {
}

CEDXDevice::~CEDXDevice() {
	if (EnableDebugLayer) {
		Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
		if (SUCCEEDED(Device.As(&debugDevice))) {
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
		}

		PIXCaptureInterface.Reset();

		if (PIXLib) {
			::FreeLibrary(PIXLib);
			PIXLib = 0;
		}

		Factory.Reset();
		Adapter.Reset();
		Device.Reset();
		DXRDevice.Reset();

		if (DXGILib) {
			::FreeLibrary(DXGILib);
			DXGILib = 0;
		}

		if (D3D12Lib) {
			::FreeLibrary(D3D12Lib);
			D3D12Lib = 0;
		}
	}
}

bool CEDXDevice::Create(Base::CreateOption option) {
	DXFunc = option;
	bool createContinue = false;
	switch (DXFunc) {
	case Base::CreateOption::DLL: {
		createContinue = CreateDLL();
	}
	break;
	case Base::CreateOption::Lib: {
		createContinue = CreateLib();
	}
	break;
	}
	if (!createContinue) {
		return false;
	}

	if (!Base::CEDirectXHandler::CECreate(option, DXGILib, D3D12Lib, PIXLib)) {
		return false;
	}

	if (EnableDebugLayer) {
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		if (FAILED(Base::CEDirectXHandler::CED3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
			CE_LOG_ERROR("[D3D12Device]: Failed to enable DebugLayer");
			return false;
		}

		debugInterface->EnableDebugLayer();

		if (EnableDRED) {
			Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
			if (SUCCEEDED(Base::CEDirectXHandler::CED3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)))) {
				dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
				dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			}
			else {
				CE_LOG_ERROR("[D3D12Device]: Failed to enable DRED");
			}
		}

		if (EnableGPUValidation) {
			Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
			if (FAILED(debugInterface.As(&debugInterface1))) {
				CE_LOG_ERROR("[D3D12Device]: Failed to enable GPU Validation");
				return false;
			}

			debugInterface1->SetEnableGPUBasedValidation(TRUE);
		}

		Microsoft::WRL::ComPtr<IDXGIInfoQueue> infoQueue;
		if (SUCCEEDED(Base::CEDirectXHandler::CEDXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue)))) {
			infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
		else {
			CE_LOG_ERROR("[D3D12Device]: Failed to retrive InfoQueue");
		}

		Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> tempPixCaptureInterface;
		if (SUCCEEDED(Base::CEDirectXHandler::CEDXGIGetDebugInterface1(0, IID_PPV_ARGS(&tempPixCaptureInterface)))) {
			PIXCaptureInterface = tempPixCaptureInterface;
		}
		else {
			CE_LOG_INFO("[D3D12Device]: PIX is not connected to applicaiton")
		}
	}

	if (FAILED(Base::CEDirectXHandler::CECreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)))) {
		CE_LOG_ERROR("[D3D12Device]: Failed to create factory");
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
	if (FAILED(Factory.As(&factory5))) {
		CE_LOG_ERROR("[D3D12Device]: Failed to retrive IDXGIFactory5");
		return false;
	}

	HRESULT hResult = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing,
	                                                sizeof(AllowTearing));
	if (SUCCEEDED(hResult)) {
		CE_LOG_INFO(AllowTearing
			? "[D32D12Device]: Tearing is supported"
			: "[D32D12Device]: Tearing is NOT supported");

	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> tempAdapter;
	for (UINT id = 0; DXGI_ERROR_NOT_FOUND != Factory->EnumAdapters1(id, &tempAdapter); id++) {
		DXGI_ADAPTER_DESC1 desc;
		if (FAILED(tempAdapter->GetDesc1(&desc))) {
			CE_LOG_ERROR("[D3D12Device]: failed to retrive DXGI_ADAPTER_DESC1");
			return false;
		}

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		if (SUCCEEDED(
			Base::CEDirectXHandler::CED3D12CreateDevice(tempAdapter.Get(), MinFeatureLevel, _uuidof(ID3D12Device),
				nullptr))) {
			AdapterID = id;
			char buff[256] = {};
			sprintf_s(buff, "[D3D12Device]: Direct3D Adapter (%u): %ls", AdapterID, desc.Description);
			CE_LOG_INFO(buff);

			break;
		}
	}

	if (!tempAdapter) {
		CE_LOG_ERROR("[D3D12Device]: failed to retrive adapter");
		return false;
	}

	Adapter = tempAdapter;

	if (FAILED(Base::CEDirectXHandler::CED3D12CreateDevice(Adapter.Get(), MinFeatureLevel, IID_PPV_ARGS(&Device)))) {
		Core::Platform::Windows::Actions::CEWindowsActions::MessageBox("Error", "Failed to create device");
		return false;
	}

	CE_LOG_INFO("[D3D12Device]: create device");

	if (EnableDebugLayer) {
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(Device.As(&infoQueue))) {
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

			D3D12_MESSAGE_ID hide[] = {
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};

			D3D12_INFO_QUEUE_FILTER filter;
			Memory::CEMemory::Memzero(&filter);

			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			infoQueue->AddStorageFilterEntries(&filter);
		}
	}

	if (FAILED(Device.As<ID3D12Device5>(&DXRDevice))) {
		CE_LOG_ERROR("[D3D12Device]: Failed to retrive DXR-Device");
		return false;
	}

	static const D3D_FEATURE_LEVEL supportedFeatureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels = {
		_countof(supportedFeatureLevels), supportedFeatureLevels, D3D_FEATURE_LEVEL_11_0
	};

	HRESULT result = Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));
	if (SUCCEEDED(result)) {
		ActiveFeatureLevel = featureLevels.MaxSupportedFeatureLevel;
	}
	else {
		ActiveFeatureLevel = MinFeatureLevel;
	}

	//RayTracing Support
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 features5;
		Memory::CEMemory::Memzero(&features5, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));

		result = Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &features5,
		                                     sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
		if (SUCCEEDED(result)) {
			RayTracingTier = features5.RaytracingTier;
		}
	}

	//Variable Shading Rate
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS6 features6;
		Memory::CEMemory::Memzero(&features6, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS6));

		result = Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &features6,
		                                     sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS6));
		if (SUCCEEDED(result)) {
			VariableShadingRateTier = features6.VariableShadingRateTier;
			VariableShadingRateTileSize = features6.ShadingRateImageTileSize;
		}
	}

	//Mesh Shader Support
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 features7;
		Memory::CEMemory::Memzero(&features7, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));
		result = Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features7,
		                                     sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));
		if (SUCCEEDED(result)) {
			MeshShaderTier = features7.MeshShaderTier;
			SamplerFeedBackTier = features7.SamplerFeedbackTier;
		}
	}

	return true;
}

int32 CEDXDevice::GetMultiSampleQuality(DXGI_FORMAT format, uint32 sampleCount) {
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS data;
	Memory::CEMemory::Memzero(&data);

	data.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	data.Format = format;
	data.SampleCount = sampleCount;

	HRESULT hr = Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &data,
	                                         sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	if (FAILED(hr)) {
		CE_LOG_ERROR("[D3D12Device] CheckFeatureSupport Failed");
		return 0;
	}

	return data.NumQualityLevels - 1;
}

std::string CEDXDevice::GetAdapterName() const {
	DXGI_ADAPTER_DESC desc;
	Adapter->GetDesc(&desc);

	const std::wstring wName = desc.Description;
	return ConvertToString(wName);
}

bool CEDXDevice::CreateLib() {

	return true;
}

bool CEDXDevice::CreateDLL() {
	DXGILib = ::LoadLibraryA("dxgi.dll");
	if (DXGILib == NULL) {
		Core::Platform::Windows::Actions::CEWindowsActions::MessageBox("Error", "Failed to load dxgi.dll");
		return false;
	}

	D3D12Lib = ::LoadLibraryA("d3d12.dll");
	if (D3D12Lib == NULL) {
		Core::Platform::Windows::Actions::CEWindowsActions::MessageBox("Error", "Failed to load d3d12.dll");
		return false;
	}

	if (EnableDebugLayer) {
		PIXLib = ::LoadLibraryA("WinPixEventRuntime.dll");
		if (PIXLib == NULL) {
			CE_LOG_WARNING("PIX Runtime NOT Loaded");
		}
	}

	return true;
}
