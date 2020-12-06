#include "CEDirect3D12Graphics.h"

#include "CEConverters.h"


void CEDirect3D12Graphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
                                             float z, CEDefaultFigureTypes figureTypes) {
}

void CEDirect3D12Graphics::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
                                              bool requestHighPerformanceAdapter) {
	*ppAdapter = nullptr;
	wrl::ComPtr<IDXGIAdapter1> adapter;
	wrl::ComPtr<IDXGIFactory6> factory;
	bool statement;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory)))) {
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapterByGpuPreference(
			     i, requestHighPerformanceAdapter == true
				        ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
				        : DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter)); ++i) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				continue;
			}
			if (SUCCEEDED(
				D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)
			)) {
				break;
			}
		}
	}
	else {
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(i, &adapter); ++i) {
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				continue;
			}
			if (SUCCEEDED(
				D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)
			)) {
				break;
			}

		}
	}

	*ppAdapter = adapter.Detach();
}

CEDirect3D12Graphics::CEDirect3D12Graphics(HWND hWnd): CEDirect3DGraphics(hWnd, CEGraphicsApiTypes::direct3d12) {
	UINT swapCreateFlags = 0u;
	HRESULT hResult;


#if defined(_DEBUG)
	{
		wrl::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			swapCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	wrl::ComPtr<IDXGIFactory4> factory;
	GFX_THROW_INFO(CreateDXGIFactory2(swapCreateFlags, IID_PPV_ARGS(&factory)));

	if (useWarpDevice) {
		wrl::ComPtr<IDXGIAdapter> warpAdapter;
		GFX_THROW_INFO(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		GFX_THROW_INFO(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&pDevice)
		));
	}
	else {
		wrl::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);
		GFX_THROW_INFO(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&pDevice)
		));
	}
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	GFX_THROW_INFO(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = 800u;
	swapChainDesc.Height = 600u;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc = {};
	swapChainFSDesc.Windowed = TRUE;

	wrl::ComPtr<IDXGISwapChain1> swapChain;
	GFX_THROW_INFO(
		factory->CreateSwapChainForHwnd(
			pCommandQueue.Get(),
			hWnd, &swapChainDesc, &swapChainFSDesc,
			nullptr, &swapChain));

	GFX_THROW_INFO(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	GFX_THROW_INFO(swapChain.As(&pSwap));

	pFrameIndex = pSwap->GetCurrentBackBufferIndex();
	{
		D3D12_DESCRIPTOR_HEAP_DESC targetViewHeapDesc = {};
		targetViewHeapDesc.NumDescriptors = FrameCount;
		targetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		targetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		GFX_THROW_INFO(pDevice->CreateDescriptorHeap(&targetViewHeapDesc, IID_PPV_ARGS(&pRtvHeap)));
		pTargetViewDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	{
		D3D12_CPU_DESCRIPTOR_HANDLE targetViewHandle(pRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < FrameCount; n++) {
			GFX_THROW_INFO(pSwap->GetBuffer(n, IID_PPV_ARGS(&pTargets[n])));
			pDevice->CreateRenderTargetView(pTargets[n].Get(), nullptr, targetViewHandle);
			targetViewHandle.ptr = SIZE_T(INT64(targetViewHandle.ptr) + INT64(1) * INT64(pTargetViewDescriptorSize));
		}
	}
	GFX_THROW_INFO(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));

	{
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.NumParameters = 0;
		rootSignatureDesc.pParameters = nullptr;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		wrl::ComPtr<ID3DBlob> signature;
		wrl::ComPtr<ID3DBlob> error;
		GFX_THROW_INFO(
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		GFX_THROW_INFO(
			pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&
				m_rootSignature)));
	}

	{
		wrl::ComPtr<ID3DBlob> pVertexShader;
		wrl::ComPtr<ID3DBlob> pPixelShader;
#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		UINT compileFlags = 0
#endif
		GFX_THROW_INFO(
			D3DCompileFromFile(GetShadersPath(L"CED3D12VertexShader.hlsl").c_str(), nullptr, nullptr, "main", "vs_5_0",
				compileFlags, 0, &pVertexShader, nullptr));
		GFX_THROW_INFO(D3DCompileFromFile(GetShadersPath(L"CED3D12PixelShader.hlsl").c_str(), nullptr, nullptr, "main",
			"ps_5_0", compileFlags, 0, &pPixelShader, nullptr));
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};


		//TODO: move all object instances to extensions of own structures
		D3D12_SHADER_BYTECODE vertexShaderBytecode;
		auto vBlob = pVertexShader.Get();
		vertexShaderBytecode.pShaderBytecode = vBlob->GetBufferPointer();
		vertexShaderBytecode.BytecodeLength = vBlob->GetBufferSize();

		D3D12_SHADER_BYTECODE pixelShaderBytecode;
		auto pBlob = pVertexShader.Get();
		pixelShaderBytecode.pShaderBytecode = pBlob->GetBufferPointer();
		pixelShaderBytecode.BytecodeLength = pBlob->GetBufferSize();

		D3D12_RASTERIZER_DESC rasterizerDescriptor;
		rasterizerDescriptor.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDescriptor.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDescriptor.FrontCounterClockwise = FALSE;
		rasterizerDescriptor.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerDescriptor.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDescriptor.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDescriptor.DepthClipEnable = TRUE;
		rasterizerDescriptor.MultisampleEnable = FALSE;
		rasterizerDescriptor.AntialiasedLineEnable = FALSE;
		rasterizerDescriptor.ForcedSampleCount = 0;
		rasterizerDescriptor.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		D3D12_BLEND_DESC blendStateDescriptor;
		blendStateDescriptor.AlphaToCoverageEnable = FALSE;
		blendStateDescriptor.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			blendStateDescriptor.RenderTarget[i] = defaultRenderTargetBlendDesc;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = vertexShaderBytecode;
		psoDesc.PS = pixelShaderBytecode;
		psoDesc.RasterizerState = rasterizerDescriptor;
		psoDesc.BlendState = blendStateDescriptor;
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
	}

	GFX_THROW_INFO(
		pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator.Get(), m_pipelineState.Get()
			, IID_PPV_ARGS(&m_commandList)));

	GFX_THROW_INFO(m_commandList->Close());

	{
		const auto aspectRatio = CEConverters::gcd((int)800, (int)600);
		// Define the geometry for a triangle.
		CEVertex triangleVertices[] =
		{
			{{0.0f, 0.25f * aspectRatio, 0.0f}},
			{{0.25f, -0.25f * aspectRatio, 0.0f}},
			{{-0.25f, -0.25f * aspectRatio, 0.0f}}
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourcesDescriptor = {
			D3D12_RESOURCE_DIMENSION_BUFFER, 0, vertexBufferSize, 1, 1, 1,
			DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE
		};

		GFX_THROW_INFO(pDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourcesDescriptor,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0;
		GFX_THROW_INFO(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(CEVertex);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;
	}

	{
		GFX_THROW_INFO(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue = 1;

		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr) {
			GFX_THROW_INFO(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}


void CEDirect3D12Graphics::EndFrame() {
	HRESULT hResult;

	const UINT64 fence = m_fenceValue;
	GFX_THROW_INFO(pCommandQueue->Signal(m_fence.Get(), fence));
	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < fence)
	{
		GFX_THROW_INFO(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	pFrameIndex = pSwap->GetCurrentBackBufferIndex();
}

void CEDirect3D12Graphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {

}
