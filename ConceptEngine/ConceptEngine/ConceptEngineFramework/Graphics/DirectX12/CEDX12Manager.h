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
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCBVDescriptorHeap() const;
		Microsoft::WRL::ComPtr<ID3D12Resource> GetDepthStencilBuffer() const;
		Microsoft::WRL::ComPtr<ID3D12Fence> GetFence() const;

		UINT64 GetFenceValue() const;
		void SetFenceValue(UINT64 newFence) ;
		
		UINT GetCurrentBackBuffer() const;
		UINT GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType);

		D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

		void ExecuteCommandLists(std::vector<ID3D12CommandList*> commandLists = {}) const;

		DXGI_FORMAT GetBackBufferFormat() const;
		DXGI_FORMAT GetDepthStencilFormat() const;

		bool GetVSync() const;
		bool GetTearingSupport() const;
		bool GetRayTracingSupport() const;
		bool GetM4XMSAAState() const;
		bool GetM4XMSAAQuality() const;
		bool IsFullScreen() const;

		float GetWindowWidth() const;
		float GetWindowHeight() const;
		HWND GetWindowHandle() const;
		float GetAspectRatio() const;

		int GetCurrentBackBufferIndex() const;
		void SetCurrentBackBufferIndex(int backBufferIndex);

		static int GetBackBufferCount();

		D3D12_VIEWPORT GetViewPort() const;
		void SetViewPort(D3D12_VIEWPORT viewPort);

		D3D12_RECT GetScissorRect() const;
		void SetScissorRect(D3D12_RECT scissorRect);

		ID3D12Resource* CurrentBackBuffer() const;
		void FlushCommandQueue();

		void ResetCommandList() const;

		/**
		 * DX Initializers
		 * TODO: Move to different classes to keep it clean. Just to test make it functions for now
		 */

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
		void CreateCSVDescriptorHeap(UINT numDescriptors = 1);
		void CreateConstantBuffers(D3D12_GPU_VIRTUAL_ADDRESS cbAddress,
		                           UINT sizeInBytes,
		                           D3D12_CPU_DESCRIPTOR_HANDLE* handle = nullptr) const;


		Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData,
		                                                           UINT64 byteSize,
		                                                           Microsoft::WRL::ComPtr<ID3D12Resource>&
		                                                           uploadBuffer) const;
		ID3D12RootSignature* CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc) const;
		ID3DBlob* CompileShaders(const std::string& fileName,
		                         const D3D_SHADER_MACRO* defines,
		                         const std::string& entryPoint,
		                         const std::string& target) const;


		// Returns random float in [0, 1).
		static float RandF() {
			return (float)(rand()) / (float)RAND_MAX;
		}

		// Returns random float in [a, b).
		static float RandF(float a, float b) {
			return a + RandF() * (b - a);
		}

		static int Rand(int a, int b) {
			return a + rand() % ((b - a) + 1);
		}

		template <typename T>
		static T Min(const T& a, const T& b) {
			return a < b ? a : b;
		}

		template <typename T>
		static T Max(const T& a, const T& b) {
			return a > b ? a : b;
		}

		template <typename T>
		static T Lerp(const T& a, const T& b, float t) {
			return a + (b - a) * t;
		}

		template <typename T>
		static T Clamp(const T& x, const T& low, const T& high) {
			return x < low ? low : (x > high ? high : x);
		}

	protected:
		friend class CEGame;
		friend class std::default_delete<CEDX12Manager>;

		CEDX12Manager(Game::CEWindow& window);
		~CEDX12Manager();
	private:

		void TearingSupport();
		void FeatureLevelSupport();
		void DirectXRayTracingSupport();
		void MultiSamplingSupport();

		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
		void LogDirectXInfo() const;

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
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[BufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;


		CEDX12Playground* m_playground;

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
