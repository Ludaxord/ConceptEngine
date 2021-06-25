#include "CEDXDevice.h"

#include <dxgi1_5.h>

#include "../../../Utilities/CEStringUtilities.h"
#include "../../../Memory/CEMemory.h"
#include "../../../Core/Log/CELog.h"
#include "../../../Core/Platform/CEPlatformActions.h"

#pragma warning(disable:4996)

using namespace ConceptEngine::Graphics::DirectX12::RenderLayer;


const char* ToString(D3D12_AUTO_BREADCRUMB_OP BreadCrumbOp) {
	switch (BreadCrumbOp) {
	case D3D12_AUTO_BREADCRUMB_OP_SETMARKER: return "D3D12_AUTO_BREADCRUMB_OP_SETMARKER";
	case D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT: return "D3D12_AUTO_BREADCRUMB_OP_BEGINEVENT";
	case D3D12_AUTO_BREADCRUMB_OP_ENDEVENT: return "D3D12_AUTO_BREADCRUMB_OP_ENDEVENT";
	case D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED: return "D3D12_AUTO_BREADCRUMB_OP_DRAWINSTANCED";
	case D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED: return "D3D12_AUTO_BREADCRUMB_OP_DRAWINDEXEDINSTANCED";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT: return "D3D12_AUTO_BREADCRUMB_OP_EXECUTEINDIRECT";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCH: return "D3D12_AUTO_BREADCRUMB_OP_DISPATCH";
	case D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION: return "D3D12_AUTO_BREADCRUMB_OP_COPYBUFFERREGION";
	case D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION: return "D3D12_AUTO_BREADCRUMB_OP_COPYTEXTUREREGION";
	case D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE: return "D3D12_AUTO_BREADCRUMB_OP_COPYRESOURCE";
	case D3D12_AUTO_BREADCRUMB_OP_COPYTILES: return "D3D12_AUTO_BREADCRUMB_OP_COPYTILES";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE: return "D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCE";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW: return "D3D12_AUTO_BREADCRUMB_OP_CLEARRENDERTARGETVIEW";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW: return "D3D12_AUTO_BREADCRUMB_OP_CLEARUNORDEREDACCESSVIEW";
	case D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW: return "D3D12_AUTO_BREADCRUMB_OP_CLEARDEPTHSTENCILVIEW";
	case D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER: return "D3D12_AUTO_BREADCRUMB_OP_RESOURCEBARRIER";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE: return "D3D12_AUTO_BREADCRUMB_OP_EXECUTEBUNDLE";
	case D3D12_AUTO_BREADCRUMB_OP_PRESENT: return "D3D12_AUTO_BREADCRUMB_OP_PRESENT";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA: return "D3D12_AUTO_BREADCRUMB_OP_RESOLVEQUERYDATA";
	case D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION: return "D3D12_AUTO_BREADCRUMB_OP_BEGINSUBMISSION";
	case D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION: return "D3D12_AUTO_BREADCRUMB_OP_ENDSUBMISSION";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME: return "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME";
	case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES: return "D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES";
	case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT: return "D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT";
	case D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64: return "D3D12_AUTO_BREADCRUMB_OP_ATOMICCOPYBUFFERUINT64";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION: return "D3D12_AUTO_BREADCRUMB_OP_RESOLVESUBRESOURCEREGION";
	case D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE: return "D3D12_AUTO_BREADCRUMB_OP_WRITEBUFFERIMMEDIATE";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1: return "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME1";
	case D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION: return
			"D3D12_AUTO_BREADCRUMB_OP_SETPROTECTEDRESOURCESESSION";
	case D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2: return "D3D12_AUTO_BREADCRUMB_OP_DECODEFRAME2";
	case D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1: return "D3D12_AUTO_BREADCRUMB_OP_PROCESSFRAMES1";
	case D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE: return
			"D3D12_AUTO_BREADCRUMB_OP_BUILDRAYTRACINGACCELERATIONSTRUCTURE";
	case D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO: return
			"D3D12_AUTO_BREADCRUMB_OP_EMITRAYTRACINGACCELERATIONSTRUCTUREPOSTBUILDINFO";
	case D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE: return
			"D3D12_AUTO_BREADCRUMB_OP_COPYRAYTRACINGACCELERATIONSTRUCTURE";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS: return "D3D12_AUTO_BREADCRUMB_OP_DISPATCHRAYS";
	case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND: return "D3D12_AUTO_BREADCRUMB_OP_INITIALIZEMETACOMMAND";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND: return "D3D12_AUTO_BREADCRUMB_OP_EXECUTEMETACOMMAND";
	case D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION: return "D3D12_AUTO_BREADCRUMB_OP_ESTIMATEMOTION";
	case D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP: return "D3D12_AUTO_BREADCRUMB_OP_RESOLVEMOTIONVECTORHEAP";
	case D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1: return "D3D12_AUTO_BREADCRUMB_OP_SETPIPELINESTATE1";
	case D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND: return
			"D3D12_AUTO_BREADCRUMB_OP_INITIALIZEEXTENSIONCOMMAND";
	case D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND: return "D3D12_AUTO_BREADCRUMB_OP_EXECUTEEXTENSIONCOMMAND";
	case D3D12_AUTO_BREADCRUMB_OP_DISPATCHMESH: return "D3D12_AUTO_BREADCRUMB_OP_DISPATCHMESH";
	default: return "UNKNOWN";
	}
}

static const char* DeviceRemovedDumpFile = "D3D12DeviceRemovedDump.txt";

void ConceptEngine::Graphics::DirectX12::RenderLayer::DeviceRemovedHandler(CEDXDevice* Device) {
	Assert(Device != nullptr);

	std::string Message = "[ConceptEngine DirectX]: Device Removed";
	CE_LOG_ERROR(Message);

	ID3D12Device* dxDevice = Device->GetDevice();

	Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedData> dred;
	if (FAILED(dxDevice->QueryInterface(IID_PPV_ARGS(&dred)))) {
		return;
	}

	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT dredAutoBreadcrumbsOutput;
	D3D12_DRED_PAGE_FAULT_OUTPUT dredPageFaultOutput;
	if (FAILED(dred->GetAutoBreadcrumbsOutput(&dredAutoBreadcrumbsOutput))) {
		return;
	}

	if (FAILED(dred->GetPageFaultAllocationOutput(&dredPageFaultOutput))) {
		return;
	}

	FILE* file = fopen(DeviceRemovedDumpFile, "w");
	if (file) {
		fwrite(Message.data(), 1, Message.size(), file);
		fputc('\n', file);
	}

	const D3D12_AUTO_BREADCRUMB_NODE* currentNode = dredAutoBreadcrumbsOutput.pHeadAutoBreadcrumbNode;
	const D3D12_AUTO_BREADCRUMB_NODE* previousNode = nullptr;
	while (currentNode) {
		Message = "Breadcrumbs: ";
		if (file) {
			fwrite(Message.data(), 1, Message.size(), file);
			fputc('\n', file);
		}

		CE_LOG_ERROR(Message);
		for (uint32 i = 0; i < currentNode->BreadcrumbCount; i++) {
			Message = "    " + std::string(ToString(currentNode->pCommandHistory[i]));
			CE_LOG_ERROR(Message);
			if (file) {
				fwrite(Message.data(), 1, Message.size(), file);
				fputc('\n', file);
			}
		}

		previousNode = currentNode;
		currentNode = currentNode->pNext;
	}

	if (file) {
		fclose(file);
	}

	CEPlatformActions::MessageBox("Error", "[ConceptEngine DirectX]: Device Removed");
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
		CE_LOG_ERROR("[CEDXDevice]: Failed to Create CEDirectXHandler")
		return false;
	}

	if (EnableDebugLayer) {
		Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
		if (FAILED(Base::CEDirectXHandler::CED3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) {
			CE_LOG_ERROR("[CEDXDevice]: Failed to enable DebugLayer");
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
				CE_LOG_ERROR("[CEDXDevice]: Failed to enable DRED");
			}
		}

		if (EnableGPUValidation) {
			Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
			if (FAILED(debugInterface.As(&debugInterface1))) {
				CE_LOG_ERROR("[CEDXDevice]: Failed to enable GPU Validation");
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
			CE_LOG_ERROR("[CEDXDevice]: Failed to retrive InfoQueue");
		}

		Microsoft::WRL::ComPtr<IDXGraphicsAnalysis> tempPixCaptureInterface;
		if (SUCCEEDED(Base::CEDirectXHandler::CEDXGIGetDebugInterface1(0, IID_PPV_ARGS(&tempPixCaptureInterface)))) {
			PIXCaptureInterface = tempPixCaptureInterface;
		}
		else {
			CE_LOG_INFO("[CEDXDevice]: PIX is not connected to applicaiton")
		}
	}

	if (FAILED(Base::CEDirectXHandler::CECreateDXGIFactory2(0, IID_PPV_ARGS(&Factory)))) {
		CE_LOG_ERROR("[CEDXDevice]: Failed to create factory");
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
	if (FAILED(Factory.As(&factory5))) {
		CE_LOG_ERROR("[CEDXDevice]: Failed to retrive IDXGIFactory5");
		return false;
	}

	HRESULT hResult = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing,
	                                                sizeof(AllowTearing));
	if (SUCCEEDED(hResult)) {
		CE_LOG_INFO(AllowTearing
			? "[CEDXDevice]: Tearing is supported"
			: "[CEDXDevice]: Tearing is NOT supported");

	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> tempAdapter;
	for (UINT id = 0; DXGI_ERROR_NOT_FOUND != Factory->EnumAdapters1(id, &tempAdapter); id++) {
		DXGI_ADAPTER_DESC1 desc;
		if (FAILED(tempAdapter->GetDesc1(&desc))) {
			CE_LOG_ERROR("[CEDXDevice]: failed to retrive DXGI_ADAPTER_DESC1");
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
			sprintf_s(buff, "[CEDXDevice]: Direct3D Adapter (%u): %ls", AdapterID, desc.Description);
			CE_LOG_INFO(buff);

			break;
		}
	}

	if (!tempAdapter) {
		CE_LOG_ERROR("[CEDXDevice]: failed to retrive adapter");
		return false;
	}

	Adapter = tempAdapter;

	if (FAILED(Base::CEDirectXHandler::CED3D12CreateDevice(Adapter.Get(), MinFeatureLevel, IID_PPV_ARGS(&Device)))) {
		Core::Platform::Windows::Actions::CEWindowsActions::MessageBox("Error", "Failed to create device");
		return false;
	}

	CE_LOG_INFO("[CEDXDevice]: create device");

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
		CE_LOG_ERROR("[CEDXDevice]: Failed to retrive DXR-Device");
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

	//Render Pass Support
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 feature5;
		Memory::CEMemory::Memzero(&feature5, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));

		result = Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &feature5,
		                                     sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
		if (SUCCEEDED(result)) {
			RenderPassTier = feature5.RenderPassesTier;
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
		CE_LOG_ERROR("[CEDXDevice] CheckFeatureSupport Failed");
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
