#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

class DirectXResources {
public:
	static const unsigned int c_AllowTearing = 0x1;
	static const unsigned int c_requireTearingSupport = 0x2;

	static bool IsDirectXRayTracingSupported(wrl::ComPtr<IDXGIAdapter1> adapter);

	DirectXResources(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
	                 DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
	                 UINT backBufferCount = 2,
	                 D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0,
	                 UINT flags = 0,
	                 UINT adapterID = UINT_MAX
	);
	~DirectXResources();

	void InitializeDXGIAdapter();
	void SetAdapterOverride(UINT adapterID) { m_adapterID = adapterID; }
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();
	void SetWindow(HWND window, int width, int height);
	void WindowSizeChanged(int width, int height, bool minimized);
	void HandleDeviceLost();
	void Prepare(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
	void Present(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);
	void ExecuteCommandList();
	void WaitForGPU() noexcept;

	RECT GetOutputSize() const { return m_outputSize; }
	bool IsWindowVisible() const { return m_isWindowVisible; }

	wrl::ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_swapChain; }
	wrl::ComPtr<IDXGIAdapter1> GetAdapter() const { return m_adapter; }
protected:
private:
	DirectXResources() {
	}

	//SwapChain Objects
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<IDXGIAdapter1> m_adapter;

	UINT m_adapterID;

	//Cached device properties.
	HWND m_window;
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;
	RECT m_outputSize;
	bool m_isWindowVisible;
};
