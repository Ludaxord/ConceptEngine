#include "CEDX12Manager.h"

#include "../../Tools/CEUtils.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;


void CEDX12Manager::Create() {
	EnableDebugLayer();
	CreateDXGIFactory();
	CheckTearingSupport();
	CreateAdapter();
	CreateDevice();
	CheckMaxFeatureLevel();
	DirectXRayTracingSupported();
}

CEDX12Manager::CEDX12Manager(): m_tearingSupported(false),
                                m_rayTracingSupported(false),
                                m_adapterIDOverride(UINT_MAX),
                                m_adapterID(UINT_MAX),
                                m_minFeatureLevel(D3D_FEATURE_LEVEL_11_0),
                                m_featureLevel(D3D_FEATURE_LEVEL_11_0) {
	spdlog::info("ConceptEngineFramework DirectX 12 class created.");
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
	HRESULT hr = D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice));

	if (FAILED(hr)) {
		wrl::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice)));
	}

#ifndef NDEBUG
	// Configure debug device (if active).
	ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue))) {
#ifdef _DEBUG
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D12_MESSAGE_ID hide[] = {
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif
}

void CEDX12Manager::CheckMaxFeatureLevel() {
	// Determine maximum supported feature level for this device
	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	HRESULT hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	m_featureLevel = SUCCEEDED(hr) ? featLevels.MaxSupportedFeatureLevel : m_minFeatureLevel;
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
		if (m_adapterIDOverride != UINT_MAX && adapterID != m_adapterIDOverride) {
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
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls", adapterID, desc.VendorId,
			           desc.DeviceId, desc.Description);
			std::string sBuff(std::begin(buff), std::end(buff));
			// spdlog::info(sBuff);
			std::wstringstream wss;
			wss << "Direct3D Adapter (" << adapterID << "): VID:" << desc.VendorId << ", PID:" << desc.DeviceId << " - GPU: "
				<< desc.Description;
			auto ws(wss.str());
			const std::string s(ws.begin(), ws.end());
			spdlog::info(s);
#endif
			break;
		}
	}

	//Create Adapter WARP
#if !defined(NDEBUG)
	if (!m_adapter && m_adapterIDOverride == UINT_MAX) {
		// Try WARP12 instead
		if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter)))) {
			spdlog::error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		spdlog::info("Direct3D Adapter - WARP 12");
	}
#endif

	if (!m_adapter) {
		if (m_adapterIDOverride != UINT_MAX) {
			throw std::exception("Unavailable adapter requested.");
		}
		else {
			throw std::exception("Unavailable adapter.");
		}
	}
}

void CEDX12Manager::DirectXRayTracingSupported() {
	ComPtr<ID3D12Device> testDevice;
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

	m_rayTracingSupported = SUCCEEDED(
			D3D12CreateDevice(
				m_adapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&testDevice)
			)
		)
		&& SUCCEEDED(
			testDevice->CheckFeatureSupport(
				D3D12_FEATURE_D3D12_OPTIONS5,
				&featureSupportData,
				sizeof(featureSupportData)
			)
		)
		&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}
