#include "CEAdapter.h"

#include "CEHelper.h"

using namespace Concept::GraphicsEngine::Direct3D;

/*
 * Instance to create object with shared_ptr
 */

class CEAdapterInstance : public CEAdapter {

public:
	explicit CEAdapterInstance(const Microsoft::WRL::ComPtr<IDXGIAdapter4>& dxgiAdapter)
		: CEAdapter(dxgiAdapter) {
	}

	virtual ~CEAdapterInstance() {

	}
};

AdapterList CEAdapter::GetAdapters(DXGI_GPU_PREFERENCE gpuPreference) {
	AdapterList adapters;

	wrl::ComPtr<IDXGIFactory6> dxgiFactory6;
	wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
	wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;

	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));

	for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
	     DXGI_ERROR_NOT_FOUND; ++i) {
		if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
		) {
			ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
			std::shared_ptr<CEAdapter> adapter = std::make_shared<CEAdapterInstance>(dxgiAdapter4);
			adapters.push_back(adapter);
		}
	}
	return adapters;
}

std::shared_ptr<CEAdapter> CEAdapter::Create(DXGI_GPU_PREFERENCE gpuPreference, bool useWarp) {
	std::shared_ptr<CEAdapter> adapter = nullptr;

	wrl::ComPtr<IDXGIFactory6> dxgiFactory6;
	wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
	wrl::ComPtr<IDXGIAdapter4> dxgiAdapter4;

	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));
	if (useWarp) {
		ThrowIfFailed(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
		ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
	}
	else {
		for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
		     DXGI_ERROR_NOT_FOUND; ++i) {
			if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
			) {
				ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
				break;
			}
		}
	}

	if (dxgiAdapter4) {
		adapter = std::make_shared<CEAdapterInstance>(dxgiAdapter4);
	}

	return adapter;
}

const std::wstring CEAdapter::GetDescription() const {
	return m_desc.Description;
}

CEAdapter::CEAdapter(wrl::ComPtr<IDXGIAdapter4> dxgiAdapter) : m_dxgiAdapter(dxgiAdapter), m_desc{0} {
	if (m_dxgiAdapter) {
		ThrowIfFailed(m_dxgiAdapter->GetDesc3(&m_desc));
	}
}
