#include "CEDirect3D12Graphics.h"


void CEDirect3D12Graphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
                                             float z, CEDefaultFigureTypes figureTypes) {
}

CEDirect3D12Graphics::CEDirect3D12Graphics(HWND hWnd): CEDirect3DGraphics(hWnd, CEGraphicsApiTypes::direct3d12) {
	DXGI_SWAP_CHAIN_DESC sd = GetSampleDescriptor(hWnd);
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
}

void CEDirect3D12Graphics::EndFrame() {
}

void CEDirect3D12Graphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {

}
