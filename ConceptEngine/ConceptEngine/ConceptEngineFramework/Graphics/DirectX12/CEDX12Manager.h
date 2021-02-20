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

namespace ConceptEngineFramework {
	namespace Game {
		class CEWindow;
	}
}

namespace wrl = Microsoft::WRL;

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager : public CEGraphicsManager {

	public:
		void Create() override;
		void Destroy() override;

	protected:
		friend class CEGame;
		friend class std::default_delete<CEDX12Manager>;

		CEDX12Manager(Game::CEWindow& window);
		~CEDX12Manager();
	private:
		void EnableDebugLayer() const;

		//TODO: Move to different classes to keep it clean. Just to test make it functions for now
		void CreateDXGIFactory();
		void CreateAdapter();
		void CreateDevice();
		void CreateFence();
		void CreateDescriptorSizes();
		void CreateCommandQueue();
		void CreateCommandAllocator();
		void CreateCommandList();
		void CreateSwapChain();

		void TearingSupport();
		void FeatureLevelSupport();
		void DirectXRayTracingSupport();
		void MultiSamplingSupport();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
		void LogDirectXInfo() const;

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;

		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> m_descriptorSizes;

		Game::CEWindow& m_window;
		
		bool m_tearingSupported;
		bool m_rayTracingSupported;
		bool m_4xMsaaState = false; // 4X MSAA enabled
		UINT m_4xMsaaQuality = 0; // quality level of 4X MSAA
		UINT m_numerator = 60;
		UINT m_denominator = 1;
		static const UINT BufferCount = 3;
		
		D3D_FEATURE_LEVEL m_minFeatureLevel;
		D3D_FEATURE_LEVEL m_featureLevel;
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		UINT m_adapterIDOverride;
		UINT m_adapterID;
		std::wstring m_adapterDescription;

	};
}
