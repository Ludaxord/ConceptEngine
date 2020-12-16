#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>



#include "CEDirect3D12Manager.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width,
                                       int height) : CEGraphics(hWnd, apiType),
                                                     m_useWarpDevice(false),
                                                     m_frameIndex(0),
                                                     m_viewport(0.0f, 0.0f, static_cast<float>(width),
                                                                static_cast<float>(height)),
                                                     m_scissorRect(0, 0, static_cast<LONG>(width),
                                                                   static_cast<LONG>(height)),
                                                     m_rtvDescriptorSize(0) {

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	if (apiType == CEOSTools::CEGraphicsApiTypes::direct3d11) {
		CreateDirect3D11(hWnd, width, height);
	}
	else if (apiType == CEOSTools::CEGraphicsApiTypes::direct3d12) {
		CreateDirect3D12(hWnd, width, height);
	}
	else {
		std::ostringstream oss;
		oss << "No API for enum: ";
		oss << magic_enum::enum_name(apiType);
		oss << std::endl;
		throw CEException(__LINE__, oss.str().c_str());
	}
}

// void CEDirect3DGraphics::EndFrame() {
// #ifndef NDEBUG
// 	infoManager.Set();
// #endif
// }

// void CEDirect3DGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
// }
//
// void CEDirect3DGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
//                                            float z, CEDefaultFigureTypes figureTypes) {
// }

//TODO: Refactor All definitions that belong to Direct3D 12 initializer

_Use_decl_annotations_

void CEDirect3DGraphics::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool requestHighPerformanceAdapter) {
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
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
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
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

void CEDirect3DGraphics::CreateDirect3D12(HWND hWnd, int width, int height) {

	//LOADING PIPELINE
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	{
		wrl::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	wrl::ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

	if (m_useWarpDevice) {
		wrl::ComPtr<IDXGIAdapter> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

		D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}
	else {
		wrl::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	wrl::ComPtr<IDXGISwapChain1> swapChain;
	factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
		hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);

	// This sample does not support fullscreen transitions.
	factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	swapChain.As(&m_swapChain);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		//TODO: Temporary solution just to run direct3d 12
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++) {
			m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

	//LOADING ASSETS
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		wrl::ComPtr<ID3DBlob> signature;
		wrl::ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		                              IID_PPV_ARGS(&m_rootSignature));
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
		auto shaderPath = GetAssetFullPath(L"BasicShaders.hlsl");
		auto sPath = boost::filesystem::path(ExePath().c_str());
		std::wstringstream wss;
		wss << "SHADER PATH:" << shaderPath << std::endl;
		wss << "SHADER PARENT PATH:" << sPath.parent_path() << std::endl;
		OutputDebugString(wss.str().c_str());
		D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0",
		                   compileFlags, 0, &vertexShader, nullptr);
		(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
		                    compileFlags, 0, &pixelShader, nullptr));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
		psoDesc.pRootSignature = m_rootSignature.Get();
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
		m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
	}

	// Create the command list.
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(),
	                            m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	m_commandList->Close();

	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		CEVertex triangleVertices[] =
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
		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto descBuffer = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		m_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&descBuffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
		m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(CEVertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		m_fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr) {
			HRESULT_FROM_WIN32(GetLastError());
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
		// WaitForPreviousFrame();
	}

}

void CEDirect3DGraphics::CreateDirect3D11(HWND hWnd, int width, int height) {
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
