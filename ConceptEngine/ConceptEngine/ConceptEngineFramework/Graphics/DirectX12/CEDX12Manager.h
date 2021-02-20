#pragma once
#include "../CEGraphicsManager.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <list>
#include <string>
#include <wrl.h>
#include <shellapi.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <atlbase.h>
#include <assert.h>
#include <array>
#include <unordered_map>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <atlbase.h>

#include <DirectXMath.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

namespace wrl = Microsoft::WRL;

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager : public CEGraphicsManager {

	public:
		void Create() override;
	protected:
		friend class CEGame;
		friend class std::default_delete<CEDX12Manager>;

		CEDX12Manager();
		~CEDX12Manager() = default;
	private:
		void EnableDebugLayer() const;

		//TODO: Move to different classes to keep it clean. Just to test make it functions for now
		void CreateDXGIFactory();
		void CheckTearingSupport();
		void CreateAdapter();
		void CreateDevice();
		void CheckMaxFeatureLevel();
		void DirectXRayTracingSupported();

		void ManagerInfo();
		
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;

		bool m_tearingSupported;
		bool m_rayTracingSupported;

		UINT m_adapterIDOverride;
		UINT m_adapterID;
		std::wstring m_adapterDescription;

		D3D_FEATURE_LEVEL m_minFeatureLevel;
		D3D_FEATURE_LEVEL m_featureLevel;
	};
}
