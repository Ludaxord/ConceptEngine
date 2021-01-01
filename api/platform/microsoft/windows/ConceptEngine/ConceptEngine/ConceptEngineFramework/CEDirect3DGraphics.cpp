#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>


#include "CEDirect3DCube.h"
#include "CEHelper.h"
#include "CETools.h"
#include "CEWindow.h"
#include "d3dx12.h"


//TODO: Test value fix CEDirect3DCube class to create vertices;
static const CEDirect3DGraphics::CEVertexPosColor g_Vertices[8] = {
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}, // 0
	{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 1
	{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}, // 2
	{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // 3
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // 4
	{{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}}, // 5
	{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}, // 6
	{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}} // 7
};

static const WORD g_Indicies[36] = {
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
};

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width,
                                       int height) : CEGraphics(hWnd, apiType, width, height),
                                                     m_rtvDescriptorSize(0),
                                                     m_Viewport(0.0f, 0.0f, static_cast<float>(width),
                                                                static_cast<float>(height)),
                                                     m_ScissorRect(0, 0, static_cast<LONG>(width),
                                                                   static_cast<LONG>(height)),
                                                     m_frameIndex(0),
                                                     m_FoV(45.0),
                                                     m_ContentLoaded(false) {

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
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	EnableDebugLayer();

	g_TearingSupported = CheckVSyncSupport();
	// Initialize the global window rect variable.
	::GetWindowRect(hWnd, &g_WindowRect);

	m_factory = GetFactory();
	m_dxgiAdapter = GetAdapter(m_useWarpDevice);
	if (m_dxgiAdapter) {
		m_device = CreateDevice(m_dxgiAdapter);
	}
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
	CreateRootSignature();
	m_commandList = CreateCommandList(m_device,
	                                  m_commandAllocator, m_pipelineState, D3D12_COMMAND_LIST_TYPE_DIRECT);
	CreateVertexBuffer();

	m_fence = CreateFence(m_device);
	m_fenceEvent = CreateEventHandle();
	Flush(m_commandQueue, m_fence, m_fenceValue, m_fenceEvent);
	// WaitForPreviousFrame();
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
                                                                             wrl::ComPtr<ID3D12PipelineState>
                                                                             pipelineState,
                                                                             D3D12_COMMAND_LIST_TYPE type) {
	wrl::ComPtr<ID3D12GraphicsCommandList> commandList;
	ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), pipelineState.Get(),
	                                        IID_PPV_ARGS(&commandList)));
	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(commandList->Close());
	return commandList;
}

void CEDirect3DGraphics::CreateRootSignature() {
	// Create an empty root signature.
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		wrl::ComPtr<ID3DBlob> signature;
		wrl::ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature,
		                                          &error));
		ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		                                            IID_PPV_ARGS(&m_RootSignature)));
	}
	// Create the pipeline state, which includes compiling and loading shaders.
	{
		wrl::ComPtr<ID3DBlob> vertexShader;
		wrl::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0;
#endif

		//Read compiled shaders
		//TODO: Create 3D object instead of Triangle
		// auto vsFile = GetAssetFullPath(L"CEVertexShader.cso");
		// auto psFile = GetAssetFullPath(L"CEPixelShader.cso");
		// ThrowIfFailed(D3DReadFileToBlob(vsFile.c_str(), &vertexShader));
		// ThrowIfFailed(D3DReadFileToBlob(psFile.c_str(), &pixelShader));

		//Add Command in Properties to copy shaders.hlsl to debug file and compile it at runtime
		auto shadersFile = GetAssetFullPath(L"shaders.hlsl");
		std::wstringstream wss;
		wss << "Shaders: " << shadersFile << std::endl;
		OutputDebugStringW(wss.str().c_str());
		ThrowIfFailed(D3DCompileFromFile(shadersFile.c_str(), nullptr, nullptr, "VSMain",
		                                 "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		ThrowIfFailed(D3DCompileFromFile(shadersFile.c_str(), nullptr, nullptr, "PSMain",
		                                 "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
		psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
	}
}

void CEDirect3DGraphics::CreateVertexBuffer() {
	CEVertexPosColor triangleVertices[] =
	{
		{{0.0f, 0.25f * m_aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
	};

	const UINT vertexBufferSize = sizeof(triangleVertices);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
	m_VertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(CEVertexPosColor);
	m_VertexBufferView.SizeInBytes = vertexBufferSize;

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
	UpdatePerSecond(1.0);

	auto fps = CountFPS(false);
	OutputDebugString(fps);
}

void CEDirect3DGraphics::UpdatePerSecond(float second) {
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	auto deltaTime = t1 - t0;
	t0 = t1;

	elapsedSeconds += deltaTime.count() * 1e-9;
	wchar_t* output = nullptr;
	if (elapsedSeconds > second) {
		// DisplayGPUInfo();
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

	// Set necessary state.
	m_commandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_commandList->RSSetViewports(1, &m_Viewport);
	m_commandList->RSSetScissorRects(1, &m_ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		                               D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
	                                        m_rtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		                               D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());
}

void CEDirect3DGraphics::OnRender() {
	// // Record all the commands we need to render the scene into the command list.
	PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	UINT syncInterval = g_VSync ? 1 : 0;
	UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	// Present the frame.
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

bool CEDirect3DGraphics::LoadContent() {
	auto cube = new CEDirect3DCube();
	return true;
}

void CEDirect3DGraphics::UnloadContent() {
}

void CEDirect3DGraphics::OnKeyPressed() {
}

void CEDirect3DGraphics::OnKeyReleased() {
}

void CEDirect3DGraphics::OnMouseMoved() {
}

void CEDirect3DGraphics::OnMouseButtonPressed() {
}

void CEDirect3DGraphics::OnMouseButtonReleased() {
}

void CEDirect3DGraphics::OnMouseWheel() {
}

void CEDirect3DGraphics::OnResize() {
}

void CEDirect3DGraphics::OnWindowDestroy() {
}

CEGraphics::IGPUInfo CEDirect3DGraphics::GetGPUInfo() {
	DXGI_QUERY_VIDEO_MEMORY_INFO dqvmi;
	wrl::ComPtr<IDXGIAdapter3> m_dxgiAdapter3;
	ThrowIfFailed(m_dxgiAdapter.As(&m_dxgiAdapter3));
	m_dxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &dqvmi);
	ID3DGPUInfo d3dGpuInfo;
	d3dGpuInfo.dqvmi = dqvmi;
	return static_cast<IGPUInfo>(d3dGpuInfo);
}

void CEDirect3DGraphics::DisplayGPUInfo() {
	const auto info = GetGPUInfo();
	std::wstringstream wss;
	wss << "GPU VRAM: " << std::endl;
	wss << "AvailableForReservation: " << ID3DGPUInfo(info).dqvmi.AvailableForReservation << std::endl;
	wss << "CurrentUsage: " << ID3DGPUInfo(info).dqvmi.CurrentUsage << std::endl;
	wss << "CurrentReservation: " << ID3DGPUInfo(info).dqvmi.CurrentReservation << std::endl;
	wss << "Budget: " << ID3DGPUInfo(info).dqvmi.Budget << std::endl;
	OutputDebugStringW(wss.str().c_str());
}

void CEDirect3DGraphics::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                            Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                                            D3D12_RESOURCE_STATES beforeState,
                                            D3D12_RESOURCE_STATES afterState) {
}

void CEDirect3DGraphics::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                  D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor) {
}

void CEDirect3DGraphics::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                    D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth) {
}

void CEDirect3DGraphics::UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
                                              ID3D12Resource** pDestinationResource,
                                              ID3D12Resource** pIntermediateResource,
                                              size_t numElements,
                                              size_t elementSize,
                                              const void* bufferData,
                                              D3D12_RESOURCE_FLAGS flags) {
	size_t bufferSize = numElements * elementSize;
	std::wstringstream wss;
	wss << "buffer size " << bufferSize << std::endl;
	wss << "num elements " << numElements << std::endl;
	wss << "element size " << elementSize << std::endl;
	OutputDebugStringW(wss.str().c_str());
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferResource = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
	ThrowIfFailed(
		m_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferResource,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(pDestinationResource)
		)
	);

	if (bufferData) {
		auto heapPropsUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDataBufferResource = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		ThrowIfFailed(
			m_device->CreateCommittedResource(
				&heapPropsUpload,
				D3D12_HEAP_FLAG_NONE,
				&bufferDataBufferResource,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(pIntermediateResource)
			)
		);
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = bufferData;
		subResourceData.RowPitch = bufferSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;
		UpdateSubresources(commandList.Get(), *pDestinationResource, *pIntermediateResource, 0, 0, 1, &subResourceData);
	}

}

void CEDirect3DGraphics::ResizeDepthBuffer(int width, int height) {
}

void CEDirect3DGraphics::CreateDirect3D12(int width, int height) {
	LoadPipeline();
	LoadAssets();
}

void CEDirect3DGraphics::CreateDirect3D11(int width, int height) {
}

void CEDirect3DGraphics::PrintGraphicsVersion() {
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
