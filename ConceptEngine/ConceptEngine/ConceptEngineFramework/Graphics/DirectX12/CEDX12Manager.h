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
	class CEDX12Playground;

	class CEDX12Manager : public CEGraphicsManager {

	public:
		void Create() override;
		void InitPlayground(CEPlayground* playground) override;
		void Destroy() override;
		void Resize() override;
		bool Initialized() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;

		Microsoft::WRL::ComPtr<IDXGIFactory4> GetDXGIFactory() const;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> GetDXGIAdapter() const;
		Microsoft::WRL::ComPtr<ID3D12Device> GetD3D12Device() const;
		Microsoft::WRL::ComPtr<ID3D12Fence> GetD3D12Fence() const;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetD3D12CommandAllocator() const;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetD3D12CommandList() const;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> GetDXGISwapChain() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTVDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDSVDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const;

		UINT GetCurrentBackBuffer() const;
		UINT GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType);

		bool GetVSync() const;
		bool GetTearingSupport() const;
		bool GetRayTracingSupport() const;
		bool IsFullScreen() const;

		int GetCurrentBackBufferIndex() const;
		void SetCurrentBackBufferIndex(int backBufferIndex);

		static int GetBackBufferCount();

		D3D12_VIEWPORT GetViewPort() const;
		void SetViewPort(D3D12_VIEWPORT viewPort);

		D3D12_RECT GetScissorRect() const;
		void SetScissorRect(D3D12_RECT scissorRect);

		ID3D12Resource* CurrentBackBuffer() const;
		void FlushCommandQueue();

	protected:
		friend class CEGame;
		friend class std::default_delete<CEDX12Manager>;

		CEDX12Manager(Game::CEWindow& window);
		~CEDX12Manager();
	private:
		//TODO: Move to different classes to keep it clean. Just to test make it functions for now

		void EnableDebugLayer() const;

		void CreateDXGIFactory();
		void CreateAdapter();
		void CreateDevice();
		void CreateFence();
		void CreateDescriptorSizes();
		void CreateCommandQueue();
		void CreateCommandAllocator();
		void CreateCommandList();
		void CreateSwapChain();
		void CreateRTVDescriptorHeap();
		void CreateDSVDescriptorHeap();

		void TearingSupport();
		void FeatureLevelSupport();
		void DirectXRayTracingSupport();
		void MultiSamplingSupport();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
		void LogDirectXInfo() const;

	private:
		static const UINT BufferCount = 3;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[BufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;

		std::shared_ptr<CEDX12Playground> m_playground;

		std::map<D3D12_DESCRIPTOR_HEAP_TYPE, UINT> m_descriptorSizes;
		UINT64 m_currentFence = 0;
		int m_currentBackBuffer = 0;

		D3D12_VIEWPORT m_screenViewport;
		D3D12_RECT m_scissorRect;

		Game::CEWindow& m_window;

		bool m_vSync = false;
		bool m_tearingSupported;
		bool m_rayTracingSupported;
		bool m_4xMsaaState = false; // 4X MSAA enabled
		UINT m_4xMsaaQuality = 0; // quality level of 4X MSAA
		UINT m_numerator = 60;
		UINT m_denominator = 1;

		D3D_FEATURE_LEVEL m_minFeatureLevel;
		D3D_FEATURE_LEVEL m_featureLevel;
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		UINT m_adapterIDOverride;
		UINT m_adapterID;
		std::wstring m_adapterDescription;

	};
}
