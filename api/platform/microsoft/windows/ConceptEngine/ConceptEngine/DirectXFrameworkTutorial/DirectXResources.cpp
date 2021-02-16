#include "DirectXResources.h"

#include <d3d12.h>
#include <dxgi1_5.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <spdlog/spdlog.h>


#include "ConceptEngineRunner.h"
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
	m_deviceNotify(nullptr),
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
	if (!m_window) {
		ThrowIfFailed(E_HANDLE, L"Call SetWindow with valid Win32 window handle");
	}

	//Wait until all previous GPU work is complete
	WaitForGPU();

	//Release resources that are tied to swap chain and update fence values
	for (UINT n = 0; n < m_backBufferCount; n++) {
		m_renderTargets[n].Reset();
		m_fenceValues[n] = m_fenceValues[m_backBufferIndex];
	}

	//Determine render target size in pixels
	UINT backBufferWidth = max(m_outputSize.right - m_outputSize.left, 1);
	UINT backBufferHeight = max(m_outputSize.bottom - m_outputSize.top, 1);
	DXGI_FORMAT backBufferFormat = NoSRGB(m_backBufferFormat);

	//if swap chain already exists, resize it , otherwise create new one;
	if (m_swapChain) {
		//if swap chain already exists, resize it;
		HRESULT hr = m_swapChain->ResizeBuffers(m_backBufferCount,
		                                        backBufferWidth,
		                                        backBufferHeight,
		                                        backBufferFormat,
		                                        (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X",
			          (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
			spdlog::error(buff);
#endif
			//If device was removed, new device and swap chain will need to be created;
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			// and correctly set up the new device.
			return;
		}
		else {
			ThrowIfFailed(hr);
		}
	}
	else {
		//Create descriptor for swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_backBufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = (m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {0};
		fsSwapChainDesc.Windowed = TRUE;

		//Create swap chain for window
		wrl::ComPtr<IDXGISwapChain1> swapChain;

		//DXGI does not allow creating swap chain targeting window which has fullscreen styles(no border + topmost).
		//Temporarily remove top most property for creating swapchain
		bool prevIsFullScreen = ConceptEngineRunner::IsFullScreen();
		if (prevIsFullScreen) {
			ConceptEngineRunner::SetWindowZOrderToTopMost(false);
		}

		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(),
		                                                    m_window,
		                                                    &swapChainDesc,
		                                                    &fsSwapChainDesc,
		                                                    nullptr,
		                                                    &swapChain));
		if (prevIsFullScreen) {
			ConceptEngineRunner::SetWindowZOrderToTopMost(true);
		}

		ThrowIfFailed(swapChain.As(&m_swapChain));

		//With tearing support enabled we will handle alt + enter key presses in window message loop rather than let DXGI handle it by calling SetFullScreenState.
		if (IsTearingSupported()) {
			m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER);
		}
	}

	//Obtain back buffers for window which will be final render targets
	//create render target views for each of them.
	for (UINT n = 0; n < m_backBufferCount; n++) {
		ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));

		wchar_t name[25] = {};
		swprintf_s(name, L"Render target %u", n);
		m_renderTargets[n]->SetName(name);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = m_backBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), n,
		                                            m_rtvDescriptorSize);
		m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), &rtvDesc, rtvDescriptor);
	}

	// Reset index to current back buffer
	m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
	if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN) {
		//Allocate 2D surface as depth/stencil buffer and create depth stencil view on surface.
		CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			m_depthBufferFormat, backBufferWidth, backBufferHeight,
			1, //Depth stencil view has only one texture 
			1 // Use single mipmap level
		);
		depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		depthOptimizedClearValue.Format = m_depthBufferFormat;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		ThrowIfFailed(m_d3dDevice->CreateCommittedResource(&depthHeapProperties,
		                                                   D3D12_HEAP_FLAG_NONE,
		                                                   &depthStencilDesc,
		                                                   D3D12_RESOURCE_STATE_DEPTH_WRITE,
		                                                   &depthOptimizedClearValue,
		                                                   IID_PPV_ARGS(&m_depthStencil)
		));

		m_depthStencil->SetName(L"Depth stencil");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = m_depthBufferFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc,
		                                    m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}

	//Set 3D rendering viewport and scissor rectangle to target entire window
	m_screenViewPort.TopLeftX = m_screenViewPort.TopLeftY = 0.f;
	m_screenViewPort.Width = static_cast<float>(backBufferWidth);
	m_screenViewPort.Height = static_cast<float>(backBufferHeight);
	m_screenViewPort.MinDepth = D3D12_MIN_DEPTH;
	m_screenViewPort.MaxDepth = D3D12_MAX_DEPTH;

	m_scissorRect.left = m_scissorRect.top = 0;
	m_scissorRect.right = backBufferWidth;
	m_scissorRect.bottom = backBufferHeight;
}

void DirectXResources::SetWindow(HWND window, int width, int height) {
	m_window = window;
	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

bool DirectXResources::WindowSizeChanged(int width, int height, bool minimized) {
	m_isWindowVisible = !minimized;
	if (minimized || width == 0 || height == 0) {
		return false;
	}

	RECT newRect;
	newRect.left = newRect.top = 0;
	newRect.right = width;
	newRect.bottom = height;
	if (newRect.left == m_outputSize.left &&
		newRect.top == m_outputSize.top &&
		newRect.right == m_outputSize.right &&
		newRect.bottom == m_outputSize.bottom) {
		return false;
	}

	m_outputSize = newRect;
	CreateWindowSizeDependentResources();
	return true;
}

void DirectXResources::HandleDeviceLost() {
	if (m_deviceNotify) {
		m_deviceNotify->OnDeviceLost();
	}

	for (UINT n = 0; n < m_backBufferCount; n ++) {
		m_commandAllocators[n].Reset();
		m_renderTargets[n].Reset();
	}

	m_depthStencil.Reset();
	m_commandQueue.Reset();
	m_commandList.Reset();
	m_fence.Reset();
	m_rtvDescriptorHeap.Reset();
	m_dsvDescriptorHeap.Reset();
	m_swapChain.Reset();
	m_d3dDevice.Reset();
	m_dxgiFactory.Reset();
	m_adapter.Reset();

#ifdef _DEBUG
	{
		wrl::ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
			                             DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
	InitializeDXGIAdapter();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();
	if (m_deviceNotify) {
		m_deviceNotify->OnDeviceRestored();
	}
}

//Prepare command list and render target for rendering
void DirectXResources::Prepare(D3D12_RESOURCE_STATES beforeState) {
	//Reset command list and allocator;
	ThrowIfFailed(m_commandAllocators[m_backBufferIndex]->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_backBufferIndex].Get(), nullptr));
	if (beforeState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
		//Transition render target into correct state to allow for drawing  into it
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_backBufferIndex].Get(), beforeState, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &barrier);
	}
}

//Present content of swap chain to screen
void DirectXResources::Present(D3D12_RESOURCE_STATES beforeState) {
	if (beforeState != D3D12_RESOURCE_STATE_PRESENT) {
		//Transition render target to state that allows it to be presented to display
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_backBufferIndex].Get(),
			beforeState,
			D3D12_RESOURCE_STATE_PRESENT
		);
		m_commandList->ResourceBarrier(1, &barrier);
	}

	ExecuteCommandList();

	HRESULT hr;
	if (m_options & c_AllowTearing) {
		//Recomented to always use tearing uf supported when using sync interval of 0
		//Note this will file if true 'fullscreen' mode;
		hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
	else {
		//First Argument instructs DXGI to block until vsync, putting applicat ion
		//to sleep mode until next vsync. This Ensures that we do not waste any cycles rendering
		//frames that will never be displayed to screen
		hr = m_swapChain->Present(1, 0);
	}

	//If device was reset we must complete reinitialize renderer
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
		char buff[64] = {};
		sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X",
		          (hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr);
		auto ss = std::string(buff);
		spdlog::error(ss);
#endif
		HandleDeviceLost();
	}
	else {
		ThrowIfFailed(hr);
		MoveToNextFrame();
	}
}

// Send the command list off to the GPU for processing.
void DirectXResources::ExecuteCommandList() {
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* commandLists[] = {
		m_commandList.Get()
	};
	m_commandQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);
}

// Wait for pending GPU work to complete.
void DirectXResources::WaitForGPU() noexcept {
	if (m_commandQueue && m_fence && m_fenceEvent.IsValid()) {
		//Schedule Signal command in GPU Queue;
		UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
		if (SUCCEEDED(m_commandQueue->Signal(m_fence.Get(), fenceValue))) {
			//Wait until Signal has been processed
			if (SUCCEEDED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get()))) {
				WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

				//Increment fence value for current frame
				m_fenceValues[m_backBufferIndex]++;
			}
		}
	}
}

//Prepare to render next frame
void DirectXResources::MoveToNextFrame() {
	//Schedule signal command in queue
	const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

	//Update back buffer index.
	m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

	//If next frame is not ready to be rendered yet. Wait until it is ready.
	if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex]) {
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
		WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
	}

	//Set fence value for next frame.
	m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
}

// This method acquires the first high-performance hardware adapter that supports Direct3D 12.
// If no such adapter can be found, try WARP. Otherwise throw an exception.
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
