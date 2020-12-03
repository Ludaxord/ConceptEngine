#include "CEDirect3DGraphics.h"


#include <d3d12.h>
#include <dxgi1_6.h>

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType): CEGraphics(hWnd, apiType) {
	if (apiType != CEGraphicsApiTypes::direct3d11 && apiType != CEGraphicsApiTypes::direct3d12) {
		throw Exception(__LINE__, "Wrong Graphics API, this class supports only Direct3D API'S");
	}
}

DXGI_SWAP_CHAIN_DESC CEDirect3DGraphics::GetSampleDescriptor(HWND hWnd) noexcept {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	return sd;
}

void CEDirect3DGraphics::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
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
			if (graphicsApiType == CEGraphicsApiTypes::direct3d11) {
				//TODO: Implement
			}
			else if (graphicsApiType == CEGraphicsApiTypes::direct3d12) {
				if (SUCCEEDED(
					D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)
				)) {
					break;
				}

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
			if (graphicsApiType == CEGraphicsApiTypes::direct3d11) {
				//TODO: Implement
			}
			else if (graphicsApiType == CEGraphicsApiTypes::direct3d12) {
				if (SUCCEEDED(
					D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)
				)) {
					break;
				}

			}
		}
	}

	*ppAdapter = adapter.Detach();
}
