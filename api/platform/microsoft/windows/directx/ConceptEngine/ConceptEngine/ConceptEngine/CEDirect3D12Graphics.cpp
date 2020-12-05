#include "CEDirect3D12Graphics.h"


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
	}
}


void CEDirect3D12Graphics::EndFrame() {
}

void CEDirect3D12Graphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {

}
