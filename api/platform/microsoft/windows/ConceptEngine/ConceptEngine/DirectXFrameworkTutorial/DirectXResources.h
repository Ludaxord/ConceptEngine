#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <spdlog/logger.h>

#include "../DirectXFrameworkTutorial/d3dx12.h"

namespace wrl = Microsoft::WRL;

using Logger = std::shared_ptr<spdlog::logger>;

// Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
interface IDeviceNotify {
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;
};

class DirectXResources {
public:
	static const unsigned int c_AllowTearing = 0x1;
	static const unsigned int c_requireTearingSupport = 0x2;

	DirectXResources(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
	                 DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
	                 UINT backBufferCount = 2,
	                 D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0,
	                 UINT flags = 0,
	                 UINT adapterID = UINT_MAX
	);
	~DirectXResources();

	void InitializeDXGIAdapter();
	void SetAdapterOverride(UINT adapterID) { m_adapterIDoverride = adapterID; }
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void SetWindow(HWND window, int width, int height);
	bool WindowSizeChanged(int width, int height, bool minimized);
	void HandleDeviceLost();
	void Prepare(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
	void Present(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);
	void ExecuteCommandList();
	void WaitForGPU() noexcept;

	void RegisterDeviceNotify(IDeviceNotify* deviceNotify) {
		m_deviceNotify = deviceNotify;
		//Application that handle device removal should declare themselves as being able to do so
		__if_exists(DXGIDeclareAdapterRemovalSupport) {
		if (deviceNotify) {
			if (FAILED(DXGIDeclareAdapterRemovalSupport())) {
				spdlog::warn("Warning: application failed to declare adapter removal support");
			}
		}
		}
	}


	//Device Accessors
	RECT GetOutputSize() const { return m_outputSize; }
	bool IsWindowVisible() const { return m_isWindowVisible; }
	bool IsTearingSupported() const { return m_options & c_AllowTearing; }

	//Direct3D Accessors
	wrl::ComPtr<IDXGIAdapter1> GetAdapter() const { return m_adapter; }
	wrl::ComPtr<ID3D12Device> GetD3DDevice() const { return m_d3dDevice; }
	wrl::ComPtr<IDXGIFactory4> GetDXGIFactory() const { return m_dxgiFactory; }
	wrl::ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_swapChain; }
	D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
	wrl::ComPtr<ID3D12Resource> GetRenderTarget() const { return m_renderTargets[m_backBufferIndex]; }
	wrl::ComPtr<ID3D12Resource> GetDepthStencil() const { return m_depthStencil; }
	wrl::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_commandQueue; }
	wrl::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const { return m_commandAllocators[m_backBufferIndex]; }
	wrl::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return m_commandList; }
	DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }
	DXGI_FORMAT GetDepthBufferFormat() const { return m_depthBufferFormat; }
	D3D12_VIEWPORT GetScreenViewPort() const { return m_screenViewPort; }
	D3D12_RECT GetScissorRect() const { return m_scissorRect; }
	UINT GetCurrentFrameIndex() const { return m_backBufferIndex; }

	UINT GetPreviousFrameIndex() const {
		return m_backBufferIndex == 0 ? m_backBufferCount - 1 : m_backBufferIndex - 1;
	}

	UINT GetBackBufferCount() const { return m_backBufferCount; }
	unsigned int GetDeviceOptions() const { return m_options; }
	std::wstring GetAdapterDescription() const { return m_adapterDescription; }
	UINT GetAdapterID() const { return m_adapterID; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		                                     m_backBufferIndex, m_rtvDescriptorSize);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}

protected:
	DirectXResources() {
	}

private:
	void MoveToNextFrame();
	void InitializeAdapter(IDXGIAdapter1** ppAdapter);

	const static size_t MAX_BACK_BUFFER_COUNT = 3;

	UINT m_adapterID;
	UINT m_adapterIDoverride;
	UINT m_backBufferIndex;
	wrl::ComPtr<IDXGIAdapter1> m_adapter;
	std::wstring m_adapterDescription;

	//Direct3D Objects
	wrl::ComPtr<ID3D12Device> m_d3dDevice;
	wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_BACK_BUFFER_COUNT];

	//SwapChain Objects
	wrl::ComPtr<IDXGIFactory4> m_dxgiFactory;
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<ID3D12Resource> m_renderTargets[MAX_BACK_BUFFER_COUNT];
	wrl::ComPtr<ID3D12Resource> m_depthStencil;

	//Presentation fence objects
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT m_fenceValues[MAX_BACK_BUFFER_COUNT];
	wrl::Wrappers::Event m_fenceEvent;

	//Direct3D rendering objects.
	wrl::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
	wrl::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
	UINT m_rtvDescriptorSize;
	D3D12_VIEWPORT m_screenViewPort;
	D3D12_RECT m_scissorRect;

	//Direct3D properties.
	DXGI_FORMAT m_backBufferFormat;
	DXGI_FORMAT m_depthBufferFormat;
	UINT m_backBufferCount;
	D3D_FEATURE_LEVEL m_d3dMinFeatureLevel;

	//Cached device properties.
	HWND m_window;
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;
	RECT m_outputSize;
	bool m_isWindowVisible;

	// DeviceResources options (see flags above)
	unsigned int m_options;

	//Device notify can be held directly as it owns directX resource
	IDeviceNotify* m_deviceNotify;
};
