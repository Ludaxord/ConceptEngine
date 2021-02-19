#include "CEDX12Manager.h"

#include "../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;


void CEDX12Manager::Create() {
	EnableDebugLayer();
}

CEDX12Manager::CEDX12Manager() {
}

void CEDX12Manager::EnableDebugLayer() const {
#if defined(DEBUG) || defined(_DEBUG)
	wrl::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
}

void CEDX12Manager::CreateDXGIFactory() {
	bool debugDXGI = false;
#if defined(_DEBUG)
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
		debugDXGI = true;
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	if (!debugDXGI) {
		CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	}
}

void CEDX12Manager::CheckTearingSupport() {
	BOOL allowTearing = FALSE;
	ComPtr<IDXGIFactory5> factory5;
	HRESULT hr = m_dxgiFactory.As(&factory5);
	if (SUCCEEDED(hr)) {
		hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
	}
	m_tearingSupported = (allowTearing == TRUE);
}


void CEDX12Manager::CreateDevice() {
	HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));

	if (FAILED(hr)) {
		wrl::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));
	}
}

void CEDX12Manager::CreateAdapter() {
	ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_dxgiFactory.As(&factory6);
	if (FAILED(hr)) {
		throw std::exception("DXGI 1.6 not supported");
	}
	//Create Adapter by GPU preference
	for (UINT adapterID = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
		     adapterID, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter)); ++adapterID) {
		if (m_adapterIDoverride != UINT_MAX && adapterID != m_adapterIDoverride) {
			continue;
		}

		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(m_adapter->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, _uuidof(ID3D12Device), nullptr))) {
			m_adapterID = adapterID;
			m_adapterDescription = desc.Description;
#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterID, desc.VendorId,
			           desc.DeviceId, desc.Description);
			std::string sBuff(std::begin(buff), std::end(buff));
			spdlog::info(sBuff);
#endif
			break;
		}
	}

	//Create Adapter WARP
#if !defined(NDEBUG)
	if (!m_adapter && m_adapterIDoverride == UINT_MAX) {
		// Try WARP12 instead
		if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter)))) {
			spdlog::error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		spdlog::info("Direct3D Adapter - WARP12\n");
	}
#endif

	if (!m_adapter) {
		if (m_adapterIDoverride != UINT_MAX) {
			throw std::exception("Unavailable adapter requested.");
		}
		else {
			throw std::exception("Unavailable adapter.");
		}
	}
}
