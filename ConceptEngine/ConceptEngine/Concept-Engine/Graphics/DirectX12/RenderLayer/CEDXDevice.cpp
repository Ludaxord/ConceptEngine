#include "CEDXDevice.h"

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
		Microsoft::WRL::ComPtr<ID3D12Debug> DebugInterface;
		if (FAILED(Base::CEDirectXHandler::CED3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface)))) {
			CE_LOG_ERROR("[D3D12Device]: Failed to enable DebugLayer");
			return false;
		}

		DebugInterface->EnableDebugLayer();
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
