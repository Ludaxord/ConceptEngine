#include "CEDXViewport.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::CEDXViewport(CEDXDevice* device,
                                                                            CEDXCommandContext* commandContext,
                                                                            HWND hwnd, CEFormat format, uint32 width,
                                                                            uint32 height) : CEViewport(format, width,
		height), CEDXDeviceElement(device), Hwnd(hwnd), SwapChain(nullptr), CommandContext(commandContext),
	BackBuffers(), BackBufferViews() {
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::~CEDXViewport() {
	BOOL FullScreenState;

	HRESULT Result = SwapChain->GetFullscreenState(&FullScreenState, nullptr);
	if (SUCCEEDED(Result)) {
		if (FullScreenState) {
			SwapChain->SetFullscreenState(FALSE, nullptr);
		}
	}
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Create() {
	
	Flags = GetDevice()->IsTearingSupported() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	Flags = Flags | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	const uint32 NumSwapChainBuffers = 3;
	const DXGI_FORMAT NativeFormat = ConvertFormat(Format);

	Assert(Width > 0 && Height > 0);

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	Memory::CEMemory::Memzero(&SwapChainDesc);

	SwapChainDesc.Width = Width;
	SwapChainDesc.Height = Height;
	SwapChainDesc.Format = NativeFormat;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = NumSwapChainBuffers;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	SwapChainDesc.Flags = Flags;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullscreenDesc;
	Memory::CEMemory::Memzero(&FullscreenDesc);

	FullscreenDesc.RefreshRate.Numerator = 0;
	FullscreenDesc.RefreshRate.Denominator = 1;
	FullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	FullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	FullscreenDesc.Windowed = true;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> TempSwapChain;
	HRESULT Result = GetDevice()->GetFactory()->CreateSwapChainForHwnd(CommandContext->GetCommandQueue().GetQueue(),
	                                                                   Hwnd, &SwapChainDesc, &FullscreenDesc, nullptr,
	                                                                   &TempSwapChain);
	if (SUCCEEDED(Result)) {
		Result = TempSwapChain.As<IDXGISwapChain3>(&SwapChain);
		if (FAILED(Result)) {
			CE_LOG_ERROR("[CEDXViewport]: Failed to Retrive IDXGISwapChain3");
			return false;
		}

		NumBackBuffers = NumSwapChainBuffers;
		SwapChain->SetMaximumFrameLatency(1);
	}
	else {
		CE_LOG_ERROR("[CEDXViewport]: Failed to create SwapChain");
		return false;
	}

	GetDevice()->GetFactory()->MakeWindowAssociation(Hwnd, DXGI_MWA_NO_ALT_ENTER);

	if (!RetrieveBackBuffers()) {
		return false;
	}

	CE_LOG_INFO("[CEDXViewport]: Created SwapChain");

	return true;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Resize(uint32 width, uint32 height) {
	if ((width != Width || height != Height) && width > 0 && height > 0) {
		CommandContext->ClearState();

		BackBuffers.Clear();
		BackBufferViews.Clear();

		HRESULT Result = SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, Flags);
		if (SUCCEEDED(Result)) {
			Width = width;
			Height = height;
		}
		else {
			CE_LOG_WARNING("[CEDXViewport]: Resize Failed");
			return false;
		}

		if (!RetrieveBackBuffers()) {
			return false;
		}
	}
	return true;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::Present(bool verticalSync) {
	const uint32 SyncInterval = !!verticalSync;

	uint32 PresentFlags = 0;
	if (SyncInterval == 0 && Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) {
		PresentFlags = DXGI_PRESENT_ALLOW_TEARING;
	}

	HRESULT Result = SwapChain->Present(SyncInterval, PresentFlags);
	if (Result == DXGI_ERROR_DEVICE_REMOVED) {
		DeviceRemovedHandler(GetDevice());
	}

	if (SUCCEEDED(Result)) {
		BackBufferIndex = SwapChain->GetCurrentBackBufferIndex();
		return true;
	}

	return false;
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::SetName(const std::string& name) {
	SwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());

	uint32 Index = 0;
	for (CERef<CEDXTexture2D>& Buffer : BackBuffers) {
		Buffer->SetName(name + " Buffer [" + std::to_string(Index) + "]");
		Index++;
	}
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXViewport::RetrieveBackBuffers() {
	if (BackBuffers.Size() < NumBackBuffers) {
		BackBuffers.Resize(NumBackBuffers);
	}

	if (BackBufferViews.Size() < NumBackBuffers) {
		CEDXOfflineDescriptorHeap* RenderTargetOfflineHeap = CastDXManager()->GetRenderTargetOfflineDescriptorHeap();
		BackBufferViews.Resize(NumBackBuffers);
		for (CERef<CEDXRenderTargetView>& View : BackBufferViews) {
			if (!View) {
				View = new CEDXRenderTargetView(GetDevice(), RenderTargetOfflineHeap);
				if (!View->Create()) {
					return false;
				}
			}
		}
	}

	for (uint32 i = 0; i < NumBackBuffers; i++) {
		Microsoft::WRL::ComPtr<ID3D12Resource> BackBufferResource;
		HRESULT Result = SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBufferResource));
		if (FAILED(Result)) {
			CE_LOG_ERROR("[CEDXViewport]: GetBuffer(" + std::to_string(i) + ") Failed");
			return false;
		}

		BackBuffers[i] = new CEDXTexture2D(GetDevice(), GetColorFormat(), Width, Height, 1, 1, 1, TextureFlag_RTV,
		                                   CEClearValue());
		BackBuffers[i]->SetResource(new CEDXResource(GetDevice(), BackBufferResource));

		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		Memory::CEMemory::Memzero(&Desc);

		Desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		Desc.Format = BackBuffers[i]->GetNativeFormat();
		Desc.Texture2D.MipSlice = 0;
		Desc.Texture2D.PlaneSlice = 0;

		if (!BackBufferViews[i]->CreateView(BackBuffers[i]->GetResource(), Desc)) {
			return false;
		}

		BackBuffers[i]->SetRenderTargetView(BackBufferViews[i].Get());
	}

	BackBufferIndex = SwapChain->GetCurrentBackBufferIndex();

	return true;
}
