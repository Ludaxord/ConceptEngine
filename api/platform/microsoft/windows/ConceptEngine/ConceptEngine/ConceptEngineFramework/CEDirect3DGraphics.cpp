#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>


#include "CEDirect3D12Manager.h"
#include "CEHelper.h"
#include "CETools.h"
#include "d3dx12.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width,
                                       int height) : CEGraphics(hWnd, apiType), g_ClientWidth(width),
                                                     g_ClientHeight(height) {

}


_Use_decl_annotations_

void CEDirect3DGraphics::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool requestHighPerformanceAdapter) const {
	*ppAdapter = nullptr;

	wrl::ComPtr<IDXGIAdapter1> adapter;

	wrl::ComPtr<IDXGIFactory6> factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
		for (
			UINT adapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true
					? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
					: DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&adapter));
			++adapterIndex) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
		}
	}
	else {
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++
		     adapterIndex) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

bool CEDirect3DGraphics::CheckVSyncSupport() const {
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
	// graphics debugging tools which will not support the 1.5 factory interface 
	// until a future update.
	wrl::ComPtr<IDXGIFactory4> factory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
		wrl::ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5))) {
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing)))) {
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}


// Load the rendering pipeline dependencies.
void CEDirect3DGraphics::LoadPipeline() {
	EnableDebugLayer();


	g_TearingSupported = CheckVSyncSupport();
	// Initialize the global window rect variable.
	::GetWindowRect(hWnd, &g_WindowRect);

	m_factory = GetFactory();
	wrl::ComPtr<IDXGIAdapter> dxgiAdapter = GetAdapter(m_useWarpDevice);
	m_device = CreateDevice(dxgiAdapter);
	m_commandQueue = CreateCommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	m_swapChain = CreateSwapChain(hWnd, m_commandQueue, g_ClientWidth, g_ClientHeight, FrameCount);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	m_rtvHeap = CreateDescriptorHeap(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount);
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UpdateRenderTargetViews(m_device, m_swapChain, m_rtvHeap);

	m_commandAllocator = CreateCommandAllocator(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

// Load the sample assets.
void CEDirect3DGraphics::LoadAssets() {
	// Create the command list.
	m_commandList = CreateCommandList(m_device,
	                                  m_commandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);

	m_fence = CreateFence(m_device);
	m_fenceEvent = CreateEventHandle();

	g_IsInitialized = true;

	// Make sure the command queue has finished all commands before closing.
	Flush(m_commandQueue, m_fence, m_fenceValue, m_fenceEvent);
}

wrl::ComPtr<IDXGIFactory4> CEDirect3DGraphics::GetFactory() const {
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	// Enable additional debug layers.
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	wrl::ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	return factory;
}

void CEDirect3DGraphics::EnableDebugLayer() {
#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		wrl::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif
}

wrl::ComPtr<IDXGIAdapter> CEDirect3DGraphics::GetAdapter(bool useWarp) const {
	wrl::ComPtr<IDXGIAdapter> dxgiAdapter;

	if (m_useWarpDevice) {
		wrl::ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
		dxgiAdapter = warpAdapter;
	}
	else {
		wrl::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(m_factory.Get(), &hardwareAdapter);
		ThrowIfFailed(hardwareAdapter.As(&dxgiAdapter));
	}

	return dxgiAdapter;
}

wrl::ComPtr<ID3D12Device> CEDirect3DGraphics::CreateDevice(wrl::ComPtr<IDXGIAdapter> adapter) const {
	wrl::ComPtr<ID3D12Device> d3d12Device;

	ThrowIfFailed(D3D12CreateDevice(
		adapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&d3d12Device)
	));
	return d3d12Device;
}

wrl::ComPtr<ID3D12CommandQueue> CEDirect3DGraphics::CreateCommandQueue(wrl::ComPtr<ID3D12Device> device,
                                                                       D3D12_COMMAND_LIST_TYPE type) const {
	wrl::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = type;
	queueDesc.NodeMask = 0;

	ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3d12CommandQueue)));
	return d3d12CommandQueue;
}

wrl::ComPtr<IDXGISwapChain3> CEDirect3DGraphics::CreateSwapChain(HWND hWnd,
                                                                 wrl::ComPtr<ID3D12CommandQueue> commandQueue,
                                                                 uint32_t width, uint32_t height,
                                                                 uint32_t bufferCount) {
	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Flags = CheckVSyncSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	//additional
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	wrl::ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
		commandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
		hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	ThrowIfFailed(m_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	wrl::ComPtr<IDXGISwapChain3> swapChain3;
	ThrowIfFailed(swapChain.As(&swapChain3));

	return swapChain3;
}

wrl::ComPtr<ID3D12DescriptorHeap> CEDirect3DGraphics::CreateDescriptorHeap(wrl::ComPtr<ID3D12Device> device,
                                                                           D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                                           uint32_t numDescriptors) const {
	wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// Describe and create a render target view (RTV) descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = type;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&descriptorHeap)));
	return descriptorHeap;
}

void CEDirect3DGraphics::UpdateRenderTargetViews(wrl::ComPtr<ID3D12Device> device,
                                                 wrl::ComPtr<IDXGISwapChain3> swapChain,
                                                 wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap) {
	// Create frame resources.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each frame.
	for (UINT n = 0; n < FrameCount; n++) {
		ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}
}

wrl::ComPtr<ID3D12CommandAllocator> CEDirect3DGraphics::CreateCommandAllocator(wrl::ComPtr<ID3D12Device> device,
                                                                               D3D12_COMMAND_LIST_TYPE type) {
	wrl::ComPtr<ID3D12CommandAllocator> commandAllocator;
	ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
	return commandAllocator;
}

wrl::ComPtr<ID3D12GraphicsCommandList> CEDirect3DGraphics::CreateCommandList(wrl::ComPtr<ID3D12Device> device,
                                                                             wrl::ComPtr<ID3D12CommandAllocator>
                                                                             commandAllocator,
                                                                             D3D12_COMMAND_LIST_TYPE type) {
	wrl::ComPtr<ID3D12GraphicsCommandList> commandList;
	ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr,
	                                        IID_PPV_ARGS(&commandList)));
	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(commandList->Close());
	return commandList;
}

wrl::ComPtr<ID3D12Fence> CEDirect3DGraphics::CreateFence(wrl::ComPtr<ID3D12Device> device) {
	wrl::ComPtr<ID3D12Fence> fence;
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	return fence;
}

HANDLE CEDirect3DGraphics::CreateEventHandle() {
	HANDLE fenceEvent;
	m_fenceValue = 1;
	// Create an event handle to use for frame synchronization.
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr) {
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}

	return fenceEvent;
}

uint64_t CEDirect3DGraphics::Signal(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
                                    uint64_t& fenceValue) {
	uint64_t fenceValueForSignal = ++fenceValue;
	ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));
	return fenceValueForSignal;

}

void CEDirect3DGraphics::WaitForFenceValue(wrl::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
                                           std::chrono::milliseconds duration) {
	if (fence->GetCompletedValue() < fenceValue) {
		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
	}
}

void CEDirect3DGraphics::Flush(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
                               uint64_t& fenceValue, HANDLE fenceEvent) {
	uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}

void CEDirect3DGraphics::OnUpdate() {
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	if (elapsedSeconds > 1.0) {
		char buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		sprintf_s(buffer, 500, "FPS: %f\n", fps);
		OutputDebugString(CETools::ConvertCharArrayToLPCWSTR(buffer));

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}
}

void CEDirect3DGraphics::PopulateCommandList() {
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(m_commandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	// Indicate that the back buffer will be used as a render target.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		                               D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
	                                        m_rtvDescriptorSize);

	// Record commands.
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		                               D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());
}

void CEDirect3DGraphics::OnRender() {
	// Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	UINT syncInterval = g_VSync ? 1 : 0;
	UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	ThrowIfFailed(m_swapChain->Present(syncInterval, presentFlags));

	WaitForPreviousFrame();
}

void CEDirect3DGraphics::Resize(uint32_t width, uint32_t height) {

}

void CEDirect3DGraphics::SetFullscreen(bool fullscreen) {

}

bool CEDirect3DGraphics::OnInit() {
	// Check for DirectX Math library support.
	if (!DirectX::XMVerifyCPUSupport()) {
		MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		CreateDirect3D11(g_ClientWidth, g_ClientHeight);
		return true;
	}
	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d12) {
		CreateDirect3D12(g_ClientWidth, g_ClientHeight);
		return true;
	}
	std::ostringstream oss;
	oss << "No API for enum: ";
	oss << magic_enum::enum_name(graphicsApiType);
	oss << std::endl;
	throw CEException(__LINE__, oss.str().c_str());
	return false;
}

void CEDirect3DGraphics::OnDestroy() {
	WaitForPreviousFrame();
	CloseHandle(m_fenceEvent);
}

void CEDirect3DGraphics::CreateDirect3D12(int width, int height) {
	LoadPipeline();
	LoadAssets();
}

void CEDirect3DGraphics::CreateDirect3D11(int width, int height) {
}

void CEDirect3DGraphics::PrintGraphicsVersion() {
}

CEGraphicsManager CEDirect3DGraphics::GetGraphicsManager() {
	if (graphicsApiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		return static_cast<CEGraphicsManager>(CEDirect3D11Manager());
	}
	else {
		return static_cast<CEGraphicsManager>(CEDirect3D12Manager());
	}
}

void CEDirect3DGraphics::WaitForPreviousFrame() {
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = m_fenceValue;
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < fence) {
		ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
