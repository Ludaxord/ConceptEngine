#include "DirectXResources.h"


#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <spdlog/spdlog.h>

#include "DirectXHelper.h"

bool DirectXResources::IsDirectXRayTracingSupported(wrl::ComPtr<IDXGIAdapter1> adapter) {
	return true;
}

DirectXResources::DirectXResources(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, UINT backBufferCount,
                                   D3D_FEATURE_LEVEL minFeatureLevel, UINT flags, UINT adapterID) :
	m_backBufferIndex(0),
	m_fenceValues{},
	m_rtvDescriptorSize(0),
	m_screenViewPort{},
	m_scissorRect{},
	m_backBufferFormat(backBufferFormat),
	m_depthBufferFormat(depthBufferFormat),
	m_backBufferCount(backBufferCount),
	m_d3dMinFeatureLevel(minFeatureLevel),
	m_outputSize{0, 0, 1, 1},
	m_options(flags),
	m_isWindowVisible(true),
	m_adapterIDoverride(adapterID),
	m_adapterID(UINT_MAX) {
	if (backBufferCount > MAX_BACK_BUFFER_COUNT) {
		throw std::out_of_range("backBufferCount too large");
	}

	if (m_options & c_requireTearingSupport) {
		m_options |= c_AllowTearing;
	}
}

DirectXResources::~DirectXResources() {
	//Ensure that GPU is no longer referencing resources that are about to be destroyed
	WaitForGPU();
}

// Configures DXGI Factory and retrieve an adapter.
void DirectXResources::InitializeDXGIAdapter() {
	bool dxgiDebug = false;
#if defined(_DEBUG)
	//Enable debug layer (requires graphics tools "optional feature")
	//NOTE: Enabling debug layer after device creation will invalidate active device
	{
		wrl::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
		else {
			spdlog::warn("WARNING: Direct3D Debug Device is not available");
		}

		wrl::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
			dxgiDebug = true;
			ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory)));

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));

	//Check Whether tearing support is available for fullscreen border less window
	if (m_options & (c_AllowTearing | c_requireTearingSupport)) {
		BOOL allowTearing = FALSE;
		wrl::ComPtr<IDXGIFactory5> factory5;
		HRESULT hr = m_dxgiFactory.As(&factory5);
		if (SUCCEEDED(hr)) {
			hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		}

		if (FAILED(hr) || !allowTearing) {
			spdlog::warn("WARNING: Variable refresh rate displays are not supported");
		}
	}

	InitializeAdapter(&m_adapter);
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DirectXResources::CreateDeviceResources() {
	//Create DX12 API Device object;
	ThrowIfFailed(D3D12CreateDevice(m_adapter.Get(), m_d3dMinFeatureLevel, IID_PPV_ARGS(&m_d3dDevice)));
#ifndef NDEBUG
	//Configure debug device (if active)
	wrl::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue))) {
#ifdef _DEBUG
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D12_MESSAGE_ID hide[] = {
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif

	//Determine maximum supported feature level for actual device
	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels = {
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));
	if (SUCCEEDED(hr)) {
		m_d3dFeatureLevel = featureLevels.MaxSupportedFeatureLevel;
	}
	else {
		m_d3dFeatureLevel = m_d3dMinFeatureLevel;
	}

	//Create command queue;
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	//Create descriptor heaps for render target views and depth stencil views.
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
	rtvDescriptorHeapDesc.NumDescriptors = m_backBufferCount;
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap)));

	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN) {
		D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
		dsvDescriptorHeapDesc.NumDescriptors = 1;
		dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

		ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&m_dsvDescriptorHeap)));
	}

	//Create command allocator for each back buffer that will be rendered to.
	for (UINT n = 0; n < m_backBufferCount; n++) {
		ThrowIfFailed(
			m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
	}

	//Create command list for recording graphics commands;
	ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(),
	                                             nullptr, IID_PPV_ARGS(&m_commandList)));
	ThrowIfFailed(m_commandList->Close());

	//Create fence for tracking GPU execution progress
	ThrowIfFailed(m_d3dDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE,
	                                       IID_PPV_ARGS(&m_fence)));
	m_fenceValues[m_backBufferIndex]++;

	m_fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
	if (!m_fenceEvent.IsValid()) {
		ThrowIfFailed(E_FAIL, L"CreateEvent Failed.");
	}
}

// These resources need to be recreated every time the window size is changed.
void DirectXResources::CreateWindowSizeDependentResources() {
}

void DirectXResources::SetWindow(HWND window, int width, int height) {
}

void DirectXResources::WindowSizeChanged(int width, int height, bool minimized) {
}

void DirectXResources::HandleDeviceLost() {
}

void DirectXResources::Prepare(D3D12_RESOURCE_STATES beforeState) {
}

void DirectXResources::Present(D3D12_RESOURCE_STATES beforeState) {
}

void DirectXResources::ExecuteCommandList() {
}

void DirectXResources::WaitForGPU() noexcept {
}

void DirectXResources::MoveToNextFrame() {
}

void DirectXResources::InitializeAdapter(IDXGIAdapter1** ppAdapter) {
	*ppAdapter = nullptr;

	wrl::ComPtr<IDXGIAdapter1> adapter;
	wrl::ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_dxgiFactory.As(&factory6);
	if (FAILED(hr)) {
		throw std::exception("DXGI 1.6 not supported");
	}
	for (UINT adapterID = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
		     adapterID, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterID) {
		if (m_adapterIDoverride != UINT_MAX && adapterID != m_adapterIDoverride) {
			continue;
		}

		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(adapter->GetDesc1(&desc));
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			//Skip software based render driver adapter
			continue;
		}

		//Check to see if adapter supports Direct3D 12, do not create device yet;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_d3dMinFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
			m_adapterID = adapterID;
			m_adapterDescription = desc.Description;

#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls", adapterID, desc.VendorId,
			           desc.DeviceId,
			           desc.Description);
			std::wstring ws(buff);
			std::string ss(ws.begin(), ws.end());
			spdlog::info(ss.c_str());
#endif
			break;
		}
	}

#if !defined(NDEBUG)
	if (!adapter && m_adapterIDoverride == UINT_MAX) {
		// Try WARP 12 instead
		if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)))) {
			throw std::exception("WARP 12 not available, enable 'Graphics Tools' optional feature");
		}
		spdlog::info("Direct3D Adapter - WARP 12");
	}
#endif

	if (!adapter) {
		if (m_adapterIDoverride != UINT_MAX) {
			throw std::exception("Unavailable adapter requested.");
		}
		else {
			throw std::exception("Unavailable adapter.");
		}
	}

	*ppAdapter = adapter.Detach();
}
