#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>


#include "CEDirect3DCube.h"
#include "CEHelper.h"
#include "CETools.h"
#include "CEWindow.h"
#include "ConceptEngine.h"
#include "d3dx12.h"


CEDirect3DGraphics::CEVertexPosColor triangleVertices[] =
{
	{{0.0f, 0.25f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.25f, -0.25f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
};

CEDirect3DGraphics::CEVertexPosColor quadVertices[] = {
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}}
};

CEDirect3DGraphics::CEVertexPosColor doubleQuadVertices[] = {
	//First quad
	{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}},
	//Second quad
	{{-0.75f, 0.75f, 0.7f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	{{0.0f, 0.0f, 0.7f}, {1.0f, 1.0f, 0.0f, 1.0f}},
	{{-0.75f, 0.0f, 0.7f}, {0.0f, 1.0f, 1.0f, 1.0f}},
	{{0.0f, 0.75f, 0.7f}, {1.0f, 1.0f, 0.0f, 1.0f}},
};


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

	adapterDescription_ = GetAdapterDescription(m_dxgiAdapter);

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

void CEDirect3DGraphics::LoadBonus() {
	std::wstring wDesc = adapterDescription_.Description;
	auto videoMem = static_cast<double>(adapterDescription_.DedicatedVideoMemory);
	std::string sDesc(wDesc.begin(), wDesc.end());
	auto nDriverVersion = adapterDescription_.AdapterLuid;
	auto deviceId = adapterDescription_.DeviceId;
	auto revision = adapterDescription_.Revision;
	auto vendorId = adapterDescription_.VendorId;

	WORD nProduct = HIWORD(nDriverVersion.HighPart);
	WORD nVersion = LOWORD(nDriverVersion.HighPart);
	WORD nSubVersion = HIWORD(nDriverVersion.LowPart);
	WORD nBuild = LOWORD(nDriverVersion.LowPart);
	ConceptEngine::GetLogger()->info("Direct3D 12 Adapter Created");


	ConceptEngine::GetLogger()->info("Device: {}, Video Memory: {:.4} MB", sDesc, fmt::format("{:.2f}", videoMem));
	ConceptEngine::GetLogger()->info("Driver: {} Build: {}.{}.{}", nProduct, nBuild, nVersion, nSubVersion);
	ConceptEngine::GetLogger()->info("DeviceID: {}", deviceId);
	ConceptEngine::GetLogger()->info("Revision: {}", revision);
	ConceptEngine::GetLogger()->info("nVendorID: {}", vendorId);

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

	if (useWarp) {
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


//TODO: Move to dynamic implementation of 3D objects, right now just to test 3d object creation in Direct3D 12
void CreateCubeRootSignature() {


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
	// CEVertexPosColor triangleVertices[] =
	// {
	// 	{{0.0f, 0.25f * m_aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
	// 	{{0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
	// 	{{-0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
	// };


	//VERTEX BUFFER
	//TODO: Create function for autocopy array
	const int len = sizeof(doubleQuadVertices) / sizeof(doubleQuadVertices[0]);
	CEVertexPosColor vertices[len];
	std::copy(std::begin(doubleQuadVertices), std::end(doubleQuadVertices), std::begin(vertices));

	const UINT vertexBufferSize = sizeof(vertices);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		// D3D12_RESOURCE_STATE_COPY_DEST, // TO USE COPY NEED TO SEND SIGNAL TO FENCE BEFORE START LOOP
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer)));
	m_VertexBuffer->SetName(L"CE Vertex Buffer Resource Heap");

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, vertices, sizeof(vertices));
	m_VertexBuffer->Unmap(0, nullptr);

	//INDEX BUFFER
	DWORD iList[] = {
		0, 1, 2, // first triangle
		0, 3, 1 // second triangle
	};
	int indexBufferSize = sizeof(iList);
	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		// D3D12_RESOURCE_STATE_COPY_DEST, // TO USE COPY NEED TO SEND SIGNAL TO FENCE BEFORE START LOOP
		nullptr,
		IID_PPV_ARGS(&m_IndexBuffer)));
	m_IndexBuffer->SetName(L"CE Index Buffer Resource Heap");

	// Copy the triangle data to the vertex buffer.
	UINT8* pIndexDataBegin;
	CD3DX12_RANGE readIndexRange(0, 0); // We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_IndexBuffer->Map(0, &readIndexRange, reinterpret_cast<void**>(&pIndexDataBegin)));
	memcpy(pIndexDataBegin, iList, sizeof(iList));
	m_IndexBuffer->Unmap(0, nullptr);
	//

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, g_ClientWidth, g_ClientHeight, 1, 0, 1, 0,
		                              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthBuffer)
	));
	m_DSVHeap->SetName(L"Depth/Stencil Resource Heap");

	m_device->CreateDepthStencilView(m_DepthBuffer.Get(), &depthStencilDesc,
	                                 m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	// Initialize the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(CEVertexPosColor);
	m_VertexBufferView.SizeInBytes = vertexBufferSize;

	//INDEXBUFFER
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IndexBufferView.SizeInBytes = indexBufferSize;

	// Fill out the Viewport
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = g_ClientWidth;
	m_Viewport.Height = g_ClientHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = g_ClientWidth;
	m_ScissorRect.bottom = g_ClientHeight;
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
	UpdateMultiplierColors();
}

void CEDirect3DGraphics::UpdateMultiplierColors() {
	// update app logic, such as moving the camera or figuring out what objects are in view
	static float rIncrement = 0.00002f;
	static float gIncrement = 0.00006f;
	static float bIncrement = 0.00009f;

	cbColorMultiplierData.colorMultiplier.x += rIncrement;
	cbColorMultiplierData.colorMultiplier.y += gIncrement;
	cbColorMultiplierData.colorMultiplier.z += bIncrement;

	if (cbColorMultiplierData.colorMultiplier.x >= 1.0 || cbColorMultiplierData.colorMultiplier.x <= 0.0) {
		cbColorMultiplierData.colorMultiplier.x = cbColorMultiplierData.colorMultiplier.x >= 1.0 ? 1.0 : 0.0;
		rIncrement = -rIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.y >= 1.0 || cbColorMultiplierData.colorMultiplier.y <= 0.0) {
		cbColorMultiplierData.colorMultiplier.y = cbColorMultiplierData.colorMultiplier.y >= 1.0 ? 1.0 : 0.0;
		gIncrement = -gIncrement;
	}
	if (cbColorMultiplierData.colorMultiplier.z >= 1.0 || cbColorMultiplierData.colorMultiplier.z <= 0.0) {
		cbColorMultiplierData.colorMultiplier.z = cbColorMultiplierData.colorMultiplier.z >= 1.0 ? 1.0 : 0.0;
		bIncrement = -bIncrement;
	}

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbColorMultiplierGPUAddress[m_frameIndex], &cbColorMultiplierData, sizeof(cbColorMultiplierData));

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
		auto fps = FPSFormula(frameCounter, elapsedSeconds);
		ConceptEngine::GetLogger()->info("FPS: {:.7}", fps);
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
	// Indicate that the back buffer will be used as a render target.
	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_renderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
	                                        m_rtvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DSVHeap->GetCPUDescriptorHandleForHeapStart());


	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	m_commandList->ClearDepthStencilView(m_DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f,
	                                     0, 0, nullptr);

	// Record commands.

	m_commandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_commandList->RSSetViewports(1, &m_Viewport);
	m_commandList->RSSetScissorRects(1, &m_ScissorRect);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_commandList->IASetIndexBuffer(&m_IndexBufferView);

	//Draw objects TODO: Create general objects to loop draw instances instead of call DrawIndexedInstanced many times
	m_commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // draw 2 triangles (draw 1 instance of 2 triangles)
	m_commandList->DrawIndexedInstanced(6, 1, 0, 4, 0); // draw second quad

	// m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		                               D3D12_RESOURCE_STATE_PRESENT));


	ThrowIfFailed(m_commandList->Close());
}


void CEDirect3DGraphics::UpdatePipeline() {
	HRESULT hr;

	// We have to wait for the gpu to finish with the command allocator before we reset it
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = m_commandAllocators[m_frameIndex]->Reset();
	if (FAILED(hr)) {
		Running = false;
	}

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	hr = m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), m_pipelineState.Get());
	if (FAILED(hr)) {
		Running = false;
	}

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
		                               D3D12_RESOURCE_STATE_RENDER_TARGET));

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
	                                        m_rtvDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// clear the depth/stencil buffer
	m_commandList->ClearDepthStencilView(m_DSVHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH,
	                                     1.0f, 0, 0, nullptr);

	// draw triangle
	m_commandList->SetGraphicsRootSignature(m_RootSignature.Get()); // set the root signature

	ID3D12DescriptorHeap* descriptorHeaps[] = {mainDescriptorHeap[m_frameIndex].Get()};
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_commandList->SetGraphicsRootDescriptorTable(
		0, mainDescriptorHeap[m_frameIndex].Get()->GetGPUDescriptorHandleForHeapStart());

	m_commandList->RSSetViewports(1, &m_Viewport); // set the viewports
	m_commandList->RSSetScissorRects(1, &m_ScissorRect); // set the scissor rects
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	m_commandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	// set the vertex buffer (using the vertex buffer view)
	m_commandList->IASetIndexBuffer(&m_IndexBufferView);
	m_commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // draw first quad
	m_commandList->DrawIndexedInstanced(6, 1, 0, 4, 0); // draw second quad

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		                               D3D12_RESOURCE_STATE_PRESENT));

	hr = m_commandList->Close();
	if (FAILED(hr)) {
		Running = false;
	}
}

void CEDirect3DGraphics::OnRender() {

	//OLD
	// // // Record all the commands we need to render the scene into the command list.
	// PopulateCommandList();
	//
	// // Execute the command list.
	// ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};
	// m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	// UINT syncInterval = g_VSync ? 1 : 0;
	// UINT presentFlags = g_TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
	// // Present the frame.
	// ThrowIfFailed(m_swapChain->Present(syncInterval, presentFlags));
	//
	// WaitForPreviousFrame();

	//NEW
	HRESULT hr;

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue

	// create an array of command lists (only one command list here)
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};

	// execute the array of command lists
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	hr = m_commandQueue->Signal(m_fences[m_frameIndex], m_fenceValues[m_frameIndex]);
	if (FAILED(hr)) {
		Running = false;
	}

	// present the current backbuffer
	hr = m_swapChain->Present(0, 0);
	if (FAILED(hr)) {
		Running = false;
	}
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

DXGI_ADAPTER_DESC CEDirect3DGraphics::GetAdapterDescription(wrl::ComPtr<IDXGIAdapter> dxgiAdapter) const {
	DXGI_ADAPTER_DESC desc = {0};
	// wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;
	// ThrowIfFailed(m_dxgiAdapter.As(&dxgiAdapter4));
	ThrowIfFailed(dxgiAdapter->GetDesc(&desc));
	// ThrowIfFailed(dxgiAdapter4->GetDesc3(&desc));
	return desc;
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
	// LoadPipeline();
	// LoadAssets();
	InitD3D12();
}

bool CEDirect3DGraphics::InitD3D12() {
	HRESULT hr;

	// -- Create the Device -- //

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return false;
	}

	IDXGIAdapter1* adapter; // adapters are the graphics card (this includes the embedded graphics on the motherboard)

	int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0

	bool adapterFound = false; // set this to true when a good one was found

	// find first hardware gpu that supports d3d 12
	while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// we dont want a software device
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr)) {
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound) {
		return false;
	}

	// Create the device
	hr = D3D12CreateDevice(
		adapter,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_device)
	);
	if (FAILED(hr)) {
		return false;
	}


	adapterDescription_ = GetAdapterDescription(adapter);
	// -- Create a direct command queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // direct means the gpu can directly execute this command queue

	hr = m_device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_commandQueue)); // create the command queue
	if (FAILED(hr)) {
		return false;
	}

	// -- Create the Swap Chain (double/tripple buffering) -- //

	DXGI_MODE_DESC backBufferDesc = {}; // this is to describe our display mode
	backBufferDesc.Width = g_ClientWidth; // buffer width
	backBufferDesc.Height = g_ClientHeight; // buffer height
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount; // number of buffers we have
	swapChainDesc.BufferDesc = backBufferDesc; // our back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// this says the pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = hWnd; // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = !g_Fullscreen;
	// set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain* tempSwapChain;

	dxgiFactory->CreateSwapChain(
		m_commandQueue.Get(), // the queue will be flushed once the swap chain is created
		&swapChainDesc, // give it the swap chain description we created above
		&tempSwapChain // store the created swap chain in a temp IDXGISwapChain interface
	);

	m_swapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount; // number of descriptors for this heap.
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	if (FAILED(hr)) {
		return false;
	}

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < FrameCount; i++) {
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		if (FAILED(hr)) {
			return false;
		}

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}

	// -- Create the Command Allocators -- //

	for (int i = 0; i < FrameCount; i++) {
		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
		if (FAILED(hr)) {
			return false;
		}
	}

	// -- Create a Command List -- //

	// create the command list with the first allocator
	hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), NULL,
	                                 IID_PPV_ARGS(&m_commandList));
	if (FAILED(hr)) {
		return false;
	}

	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < FrameCount; i++) {
		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fences[i]));
		if (FAILED(hr)) {
			return false;
		}
		m_fenceValues[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		return false;
	}

	// create root signature
	D3D12_DESCRIPTOR_RANGE descriptorTableRange[1];
	descriptorTableRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descriptorTableRange[0].NumDescriptors = 1;
	descriptorTableRange[0].BaseShaderRegister = 0;
	descriptorTableRange[0].RegisterSpace = 0;
	descriptorTableRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRange);
	descriptorTable.pDescriptorRanges = &descriptorTableRange[0];

	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(
		_countof(rootParameters),
		rootParameters,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
	);

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr)) {
		return false;
	}

	hr = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
	                                   IID_PPV_ARGS(&m_RootSignature));
	if (FAILED(hr)) {
		return false;
	}

	// create vertex and pixel shaders

	// when debugging, we can compile the shader files at runtime.
	// but for release versions, we can compile the hlsl shaders
	// with fxc.exe to create .cso files, which contain the shader
	// bytecode. We can load the .cso files at runtime to get the
	// shader bytecode, which of course is faster than compiling
	// them at runtime
	wrl::ComPtr<ID3DBlob> vertexShader;
	wrl::ComPtr<ID3DBlob> pixelShader;
	ID3DBlob* errorBuff; // a buffer holding the error data if any

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
	                                 "vs_5_0", compileFlags, 0, &vertexShader, &errorBuff));


	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

	ThrowIfFailed(D3DCompileFromFile(shadersFile.c_str(), nullptr, nullptr, "PSMain",
	                                 "ps_5_0", compileFlags, 0, &pixelShader, &errorBuff));
	if (FAILED(hr)) {
		OutputDebugStringA((char*)errorBuff->GetBufferPointer());
		return false;
	}

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = m_RootSignature.Get(); // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode;
	// structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff;
	// sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state

	// create the pso
	hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	if (FAILED(hr)) {
		return false;
	}

	// Create vertex buffer

	// a triangle
	// Vertex vList[] = {
	// 	// first quad (closer to camera, blue)
	// 	{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f},
	// 	{0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f},
	// 	{-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f},
	// 	{0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f},
	//
	// 	// second quad (further from camera, green)
	// 	{-0.75f, 0.75f, 0.7f, 0.0f, 1.0f, 0.0f, 1.0f},
	// 	{0.0f, 0.0f, 0.7f, 0.0f, 1.0f, 0.0f, 1.0f},
	// 	{-0.75f, 0.0f, 0.7f, 0.0f, 1.0f, 0.0f, 1.0f},
	// 	{0.0f, 0.75f, 0.7f, 0.0f, 1.0f, 0.0f, 1.0f}
	// };

	const int len = sizeof(doubleQuadVertices) / sizeof(doubleQuadVertices[0]);
	CEVertexPosColor vList[len];
	std::copy(std::begin(doubleQuadVertices), std::end(doubleQuadVertices), std::begin(vList));

	int vBufferSize = sizeof(vList);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
		// from the upload heap to this heap
		nullptr,
		// optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&m_VertexBuffer));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(m_commandList.Get(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Create index buffer

	// a quad (2 triangles)
	DWORD iList[] = {
		// first quad (blue)
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle
	};

	int iBufferSize = sizeof(iList);

	// create default heap to hold index buffer
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
		nullptr, // optimized clear value must be null for this type of resource
		IID_PPV_ARGS(&m_IndexBuffer));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	m_VertexBuffer->SetName(L"Index Buffer Resource Heap");

	// create upload heap to upload index buffer
	ID3D12Resource* iBufferUploadHeap;
	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&iBufferUploadHeap));
	iBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(m_commandList.Get(), m_IndexBuffer.Get(), iBufferUploadHeap, 0, 0, 1, &indexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		                               m_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		                               D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// Create the depth/stencil buffer

	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap));
	if (FAILED(hr)) {
		Running = false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, g_ClientWidth, g_ClientHeight, 1, 0, 1, 0,
		                              D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DepthBuffer)
	);
	m_DSVHeap->SetName(L"Depth/Stencil Resource Heap");

	m_device->CreateDepthStencilView(m_DepthBuffer.Get(), &depthStencilDesc,
	                                 m_DSVHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < FrameCount; ++i) {
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 1;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mainDescriptorHeap[i]));
		if (FAILED(hr)) {
			Running = false;
		}
	}

	for (int i = 0; i < FrameCount; ++i) {
		hr = m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constantBufferUploadHeap[i])
		);
		constantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constantBufferUploadHeap[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (sizeof(ConstantBuffer) + 255) & ~255;
		m_device->CreateConstantBufferView(&cbvDesc, mainDescriptorHeap[i].Get()->GetCPUDescriptorHandleForHeapStart());
		ZeroMemory(&cbColorMultiplierData, sizeof(cbColorMultiplierData));

		CD3DX12_RANGE readRange(0, 0);
		hr = constantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbColorMultiplierGPUAddress[i]));
		memcpy(cbColorMultiplierGPUAddress[i], &cbColorMultiplierData, sizeof(cbColorMultiplierData));
	}

	// Now we execute the command list to upload the initial assets (triangle data)
	m_commandList->Close();
	ID3D12CommandList* ppCommandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	m_fenceValues[m_frameIndex]++;
	hr = m_commandQueue->Signal(m_fences[m_frameIndex], m_fenceValues[m_frameIndex]);
	if (FAILED(hr)) {
		Running = false;
	}

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(Vertex);
	m_VertexBufferView.SizeInBytes = vBufferSize;

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	m_IndexBufferView.SizeInBytes = iBufferSize;

	// Fill out the Viewport
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.Width = g_ClientWidth;
	m_Viewport.Height = g_ClientHeight;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = g_ClientWidth;
	m_ScissorRect.bottom = g_ClientHeight;

	return true;
}

void CEDirect3DGraphics::CreateDirect3D11(int width, int height) {
}

void CEDirect3DGraphics::PrintGraphicsVersion() {
}

void CEDirect3DGraphics::WaitForPreviousFrame() {
	// // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// // sample illustrates how to use fences for efficient resource usage and to
	// // maximize GPU utilization.
	//
	// // Signal and increment the fence value.
	// const UINT64 fence = m_fenceValue;
	// ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
	// m_fenceValue++;
	//
	// // Wait until the previous frame is finished.
	// if (m_fence->GetCompletedValue() < fence) {
	// 	ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
	// 	WaitForSingleObject(m_fenceEvent, INFINITE);
	// }
	//
	// m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (m_fences[m_frameIndex]->GetCompletedValue() < m_fenceValues[m_frameIndex]) {
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		hr = m_fences[m_frameIndex]->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
		if (FAILED(hr)) {
			Running = false;
		}

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	// increment fenceValue for next frame
	m_fenceValues[m_frameIndex]++;
}
