#include "CEDirect3DGraphics.h"

#include <magic_enum.hpp>


#include "CEDirect3D12Manager.h"
#include "CEHelper.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width,
                                       int height) : CEGraphics(hWnd, apiType) {

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

void CEDirect3DGraphics::OnRender() {
}

wrl::ComPtr<IDXGIAdapter4> CEDirect3DGraphics::GetAdapter(bool useWarp) const {
	wrl::ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));
	wrl::ComPtr<IDXGIAdapter1> dxgiAdapter1;
	wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;

	if (useWarp) {
		ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
		ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
	}
	else {
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if (
				(dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0
				&& SUCCEEDED(
					D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr))
				&& dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory
			) {
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
			}
		}
	}

	return dxgiAdapter4;
}

wrl::ComPtr<ID3D12Device2> CEDirect3DGraphics::Cr\eateDevice(wrl::ComPtr<IDXGIAdapter4> adapter) const {
	wrl::ComPtr<ID3D12Device2> d3d12Device2;
	ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3d12Device2)));

	// Enable debug messages in debug mode.
#if defined(_DEBUG)
	wrl::ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(d3d12Device2.As(&pInfoQueue))) {
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] = {
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			// I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			// This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			// This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}
#endif

	return d3d12Device2;
}

wrl::ComPtr<ID3D12CommandQueue> CEDirect3DGraphics::CreateCommandQueue(wrl::ComPtr<ID3D12Device2> device,
                                                                       D3D12_COMMAND_LIST_TYPE type) const {
	wrl::ComPtr<ID3D12CommandQueue> d3d12CommandQueue;
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));
	return d3d12CommandQueue;
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

wrl::ComPtr<IDXGISwapChain4> CEDirect3DGraphics::CreateSwapChain(
	HWND hWnd, wrl::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width,
	uint32_t height, uint32_t bufferCount) {
	wrl::ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	wrl::ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = {1, 0};
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = CheckVSyncSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	wrl::ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1));

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	return dxgiSwapChain4;
}

wrl::ComPtr<ID3D12DescriptorHeap> CEDirect3DGraphics::CreateDescriptorHeap(wrl::ComPtr<ID3D12Device2> device,
                                                       D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) const {
	wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}

void CEDirect3DGraphics::CreateDirect3D12(HWND hWnd, int width, int height) {
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

void CEDirect3DGraphics::EnableDebugLayer() {
#if defined(_DEBUG)
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	wrl::ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}
