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
#if defined( _DEBUG )
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));

	for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
	     DXGI_ERROR_NOT_FOUND;
	     ++i) {
		if (SUCCEEDED(
			D3D12CreateDevice(dxgiAdapter.Get(), m_d3dMinimumFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
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

	wrl::ComPtr<ID3D12Device> featureCheckDevice = nullptr;

	UINT createFactoryFlags = 0;
#if defined( _DEBUG )
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));
	if (useWarp) {
		ThrowIfFailed(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
		ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
	}
	else {
		for (UINT i = 0; dxgiFactory6->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) !=
		     DXGI_ERROR_NOT_FOUND;
		     ++i) {
			if (SUCCEEDED(
				// D3D12CreateDevice(dxgiAdapter.Get(), m_d3dMinimumFeatureLevel, __uuidof(ID3D12Device), nullptr)
				D3D12CreateDevice(dxgiAdapter.Get(), m_d3dMinimumFeatureLevel, IID_PPV_ARGS(&featureCheckDevice))
			)) {
				ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
				break;
			}
		}
	}

	if (dxgiAdapter4) {
		adapter = std::make_shared<CEAdapterInstance>(dxgiAdapter4);
	}

	adapter->SetFeatureLevel(featureCheckDevice);

	return adapter;
}

const std::wstring CEAdapter::GetDescription() const {
	return m_desc.Description;
}

D3D_FEATURE_LEVEL CEAdapter::GetFeatureLevel() const {
	return m_d3dFeatureLevel;
}

std::string CEAdapter::GetFeatureLevelName() const {
	switch (m_d3dFeatureLevel) {
	case D3D_FEATURE_LEVEL_12_1:
		return "12.1";
	case D3D_FEATURE_LEVEL_12_0:
		return "12.0";
	case D3D_FEATURE_LEVEL_11_1:
		return "11.1";
	case D3D_FEATURE_LEVEL_11_0:
		return "11.0";
	default:
		return "Unknown";
	}
}

CEAdapter::CEAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter): m_dxgiAdapter(dxgiAdapter)
                                                                         , m_desc{0} {
	if (m_dxgiAdapter) {
		ThrowIfFailed(m_dxgiAdapter->GetDesc3(&m_desc));
	}
}

void CEAdapter::SetFeatureLevel(wrl::ComPtr<ID3D12Device> device) {
	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		_countof(m_featureLevels), m_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels)))) {
		m_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;
	}
	else {
		m_d3dFeatureLevel = m_d3dMinimumFeatureLevel;
	}
}
