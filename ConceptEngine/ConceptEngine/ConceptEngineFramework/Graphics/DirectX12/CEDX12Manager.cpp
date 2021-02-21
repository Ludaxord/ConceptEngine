#include "CEDX12Manager.h"


#include "../../Game/CEWindow.h"
#include "../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

void CEDX12Manager::Create() {
	EnableDebugLayer();
	CreateDXGIFactory();
	TearingSupport();
	CreateAdapter();
	CreateDevice();
	FeatureLevelSupport();
	DirectXRayTracingSupport();
	CreateFence();
	CreateDescriptorSizes();
	MultiSamplingSupport();

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandQueue();
	CreateCommandAllocator();
	CreateCommandList();
	CreateSwapChain();
	CreateRTVDescriptorHeap();
	CreateDSVDescriptorHeap();

	LogDirectXInfo();

	Resize();
}

void CEDX12Manager::Destroy() {
#ifdef _DEBUG
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
			                             DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void CEDX12Manager::Resize() {
	assert(m_d3dDevice);
	assert(m_swapChain);
	assert(m_commandAllocator);

	//Flush before changing resources
	FlushCommandQueue();

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	//Release previous resources that will be recreated
	for (int i = 0; i < BufferCount; ++i) {
		m_swapChainBuffer[i].Reset();
	}
	m_depthStencilBuffer.Reset();

	//Resize swap chain
	auto swapChainFlags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ThrowIfFailed(m_swapChain->ResizeBuffers(BufferCount,
	                                         m_window.GetWidth(),
	                                         m_window.GetHeight(),
	                                         m_backBufferFormat,
	                                         swapChainFlags));
	m_currentBackBuffer = 0;

	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//Create render target buffer and view
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < BufferCount; i++) {
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));
		m_d3dDevice->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_descriptorSizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]);
	}

	//Create depth stencil buffer and view
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_window.GetWidth();
	depthStencilDesc.Height = m_window.GetHeight();
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = m_depthStencilFormat;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
	));

	//Create descriptor to mip level 0 of entire resource using format of resource
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc,
	                                    m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//Transition resource from internal state to be used as depth buffer
	auto dsvTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandList->ResourceBarrier(1, &dsvTransitionBarrier);

	//Execute resize commands
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* commandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until resize if complete
	FlushCommandQueue();

	//Update viewport transform to cover client area

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = static_cast<float>(m_window.GetWidth());
	m_screenViewport.Height = static_cast<float>(m_window.GetHeight());
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_scissorRect = {0, 0, m_window.GetWidth(), m_window.GetHeight()};
}

bool CEDX12Manager::Initialized() {
	return m_d3dDevice;
}

CEDX12Manager::CEDX12Manager(Game::CEWindow& window) : m_window(window),
                                                       m_tearingSupported(false),
                                                       m_rayTracingSupported(false),
                                                       m_adapterIDOverride(UINT_MAX),
                                                       m_adapterID(UINT_MAX),
                                                       m_minFeatureLevel(D3D_FEATURE_LEVEL_11_0),
                                                       m_featureLevel(D3D_FEATURE_LEVEL_11_0) {
	spdlog::info("ConceptEngineFramework DirectX 12 class created.");
}


CEDX12Manager::~CEDX12Manager() {
	CEDX12Manager::Destroy();
}

void CEDX12Manager::FlushCommandQueue() {
	//Advance fence value to mark commands up to fence point;
	m_currentFence++;

	//Add instruction to command queue to set new fence point. Because we are on GPU timeline, new fence point will not be set until GPU finishes processign all commands prior to Signal()
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFence));

	//Wait until GPU has completed commands up to this fence point
	if (m_fence->GetCompletedValue() < m_currentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

		//Fire event when GPU hits current fence;
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));

		//Wait until GPU hits current fence event is fired.
		WaitForSingleObjectEx(eventHandle, 1000, TRUE);
	}
}

void CEDX12Manager::EnableDebugLayer() const {
#if defined(DEBUG) || defined(_DEBUG)
	wrl::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
}

/*
 * Objects Creators
 */
void CEDX12Manager::CreateDXGIFactory() {
	bool debugDXGI = false;
#if defined(_DEBUG)
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
		debugDXGI = true;
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	if (!debugDXGI) {
		CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	}
}

void CEDX12Manager::CreateDevice() {
	HRESULT hr = D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice));

	if (FAILED(hr)) {
		wrl::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice)));
	}

#ifndef NDEBUG
	// Configure debug device (if active).
	ComPtr<ID3D12InfoQueue> d3dInfoQueue;
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
}

void CEDX12Manager::CreateAdapter() {
	ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_dxgiFactory.As(&factory6);
	if (FAILED(hr)) {
		throw std::exception("DXGI 1.6 not supported");
	}
	//Create Adapter by GPU preference
	for (UINT adapterID = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
		     adapterID, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter)); ++adapterID) {
		if (m_adapterIDOverride != UINT_MAX && adapterID != m_adapterIDOverride) {
			continue;
		}

		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(m_adapter->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, _uuidof(ID3D12Device), nullptr))) {
			m_adapterID = adapterID;
			m_adapterDescription = desc.Description;
#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls", adapterID, desc.VendorId,
			           desc.DeviceId, desc.Description);
			std::string sBuff(std::begin(buff), std::end(buff));
			// spdlog::info(sBuff);
			std::wstringstream wss;
			wss << "Direct3D Adapter (" << adapterID << "): VID:" << desc.VendorId << ", PID:" << desc.DeviceId <<
				" - GPU: "
				<< desc.Description;
			auto ws(wss.str());
			const std::string s(ws.begin(), ws.end());
			spdlog::info(s);
#endif
			break;
		}
	}

	//Create Adapter WARP
#if !defined(NDEBUG)
	if (!m_adapter && m_adapterIDOverride == UINT_MAX) {
		// Try WARP12 instead
		if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter)))) {
			spdlog::error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		spdlog::info("Direct3D Adapter - WARP 12");
	}
#endif

	if (!m_adapter) {
		if (m_adapterIDOverride != UINT_MAX) {
			throw std::exception("Unavailable adapter requested.");
		}
		else {
			throw std::exception("Unavailable adapter.");
		}
	}
}

void CEDX12Manager::CreateFence() {
	ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}

void CEDX12Manager::CreateDescriptorSizes() {
	m_descriptorSizes = {
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		},
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		},
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		}
	};
}

void CEDX12Manager::CreateCommandQueue() {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
}

void CEDX12Manager::CreateCommandAllocator() {
	ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_commandAllocator.GetAddressOf())
		)
	);

}

void CEDX12Manager::CreateCommandList() {
	ThrowIfFailed(m_d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(), // Associated command allocator
			nullptr, // Initial PipelineStateObject
			IID_PPV_ARGS(m_commandList.GetAddressOf())
		)
	);

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_commandList->Close();
}

void CEDX12Manager::CreateSwapChain() {
	m_swapChain.Reset();

	// Get the factory that was used to create the adapter.
	wrl::ComPtr<IDXGIFactory> dxgiFactory;
	wrl::ComPtr<IDXGIFactory5> dxgiFactory5;
	ThrowIfFailed(m_adapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
	// Now get the DXGIFactory5 so I can use the IDXGIFactory5::CheckFeatureSupport method.
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory5));

	RECT windowRect;
	::GetClientRect(m_window.GetWindowHandle(), &windowRect);

	m_window.SetWidth(windowRect.right - windowRect.left);
	m_window.SetHeight(windowRect.bottom - windowRect.top);

	auto* hwnd = m_window.GetWindowHandle();

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_window.GetWidth();
	swapChainDesc.Height = m_window.GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {m_numerator, m_denominator};
	fsSwapChainDesc.Windowed = TRUE;
	// Now create the swap chain.
	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
	ThrowIfFailed(dxgiFactory5->CreateSwapChainForHwnd(m_commandQueue.Get(),
	                                                   hwnd,
	                                                   &swapChainDesc,
	                                                   &fsSwapChainDesc,
	                                                   nullptr,
	                                                   &dxgiSwapChain1));
	ThrowIfFailed(dxgiSwapChain1.As(&m_swapChain));
}

void CEDX12Manager::CreateRTVDescriptorHeap() {
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = BufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())));
}

void CEDX12Manager::CreateDSVDescriptorHeap() {
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));
}

/*
 * Support functions
 */
void CEDX12Manager::TearingSupport() {
	BOOL allowTearing = FALSE;
	ComPtr<IDXGIFactory5> factory5;
	HRESULT hr = m_dxgiFactory.As(&factory5);
	if (SUCCEEDED(hr)) {
		hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
	}
	m_tearingSupported = (allowTearing == TRUE);
}

void CEDX12Manager::FeatureLevelSupport() {
	// Determine maximum supported feature level for this device
	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	HRESULT hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	m_featureLevel = SUCCEEDED(hr) ? featLevels.MaxSupportedFeatureLevel : m_minFeatureLevel;
}

void CEDX12Manager::DirectXRayTracingSupport() {
	ComPtr<ID3D12Device> testDevice;
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

	m_rayTracingSupported = SUCCEEDED(
			D3D12CreateDevice(
				m_adapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&testDevice)
			)
		)
		&& SUCCEEDED(
			testDevice->CheckFeatureSupport(
				D3D12_FEATURE_D3D12_OPTIONS5,
				&featureSupportData,
				sizeof(featureSupportData)
			)
		)
		&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

void CEDX12Manager::MultiSamplingSupport() {
	// Check 4X MSAA quality support for back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)
	));
	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}

/*
 * Log functions
 */
void CEDX12Manager::LogAdapters() {
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring adapterInfo = L"*** Adapter: ";
		adapterInfo += desc.Description;
		std::string sAdapterInfo(adapterInfo.begin(), adapterInfo.end());
		spdlog::info(sAdapterInfo);

		adapterList.push_back(adapter);
		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i) {
		LogAdapterOutputs(adapterList[i]);
		adapterList[i]->Release();
	}
}

void CEDX12Manager::LogAdapterOutputs(IDXGIAdapter* adapter) {
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring outputInfo = L"*** Output: ";
		outputInfo += desc.DeviceName;
		std::string sOutputInfo(outputInfo.begin(), outputInfo.end());
		spdlog::info(sOutputInfo);

		LogOutputDisplayModes(output, m_backBufferFormat);

		output->Release();

		++i;
	}
}

void CEDX12Manager::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format) {
	UINT count = 0;
	UINT flags = 0;

	//Call with nullptr to get list count;
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& mode : modeList) {
		UINT numerator = mode.RefreshRate.Numerator;
		UINT denominator = mode.RefreshRate.Denominator;
		if (numerator > m_numerator) {
			m_numerator = numerator;
		}
		if (denominator > m_denominator) {
			m_denominator = denominator;
		}

		std::wstring modeInfo = L"*** DisplayMode: Width = " + std::to_wstring(mode.Width) + L" " +
			L"Height = " + std::to_wstring(mode.Height) + L" " +
			L"Refresh = " + std::to_wstring(numerator) + L"/" + std::to_wstring(denominator);
		std::string sModeInfo(modeInfo.begin(), modeInfo.end());
		spdlog::info(sModeInfo);
	}
}

void CEDX12Manager::LogDirectXInfo() const {

	std::string minFeatureLevelName;
	std::string featureLevelName;

	switch (m_minFeatureLevel) {
	case D3D_FEATURE_LEVEL_12_1:
		minFeatureLevelName = "12.1";
		break;
	case D3D_FEATURE_LEVEL_12_0:
		minFeatureLevelName = "12.0";
		break;
	case D3D_FEATURE_LEVEL_11_1:
		minFeatureLevelName = "11.1";
		break;
	case D3D_FEATURE_LEVEL_11_0:
		minFeatureLevelName = "11.0";
		break;
	default:
		minFeatureLevelName = "NO DATA";
		break;
	}

	switch (m_featureLevel) {
	case D3D_FEATURE_LEVEL_12_1:
		featureLevelName = "12.1";
		break;
	case D3D_FEATURE_LEVEL_12_0:
		featureLevelName = "12.0";
		break;
	case D3D_FEATURE_LEVEL_11_1:
		featureLevelName = "11.1";
		break;
	case D3D_FEATURE_LEVEL_11_0:
		featureLevelName = "11.0";
		break;
	default:
		featureLevelName = "NO DATA";
		break;
	}

	const auto* const rayTracingSupported = m_rayTracingSupported ? "TRUE" : "FALSE";
	const auto* const tearingSupported = m_tearingSupported ? "TRUE" : "FALSE";
	spdlog::info("Min Feature Level Support: {}", minFeatureLevelName);
	spdlog::info("Max Feature Level Support: {}", featureLevelName);
	spdlog::info("Ray Tracing Support: {}", rayTracingSupported);
	spdlog::info("Tearing Support: {}", tearingSupported);
	spdlog::info("4x MSAA (MultiSampling) Quality: {}", m_4xMsaaQuality);
	spdlog::info("RefreshRate: {}/{}", m_numerator, m_denominator);
}
