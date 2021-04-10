#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "../ConceptEngineFramework/Graphics/DirectX12/Libraries/d3dx12.h"
#include "CEDX12Widget.h"
#include <QtWidgets/private/qwidget_p.h>
#include <wrl/client.h>

class CEDX12WidgetPrivate final : public QWidgetPrivate {
	Q_DECLARE_PUBLIC(CEDX12Widget)

public:
	explicit CEDX12WidgetPrivate(CEDX12Widget* parent);
	~CEDX12WidgetPrivate();

private:
	// Init DirectX12 3D.
	void Create();

	// DirectX12 3D Render.
	void Render();

	// Handle viewport size when window size change.
	void Resize();

	// Helper function for resolving the full path of assets.
	QString GetAssetsPath(const QString& assetName) const { return m_assetsPath + assetName; }

	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, *ppAdapter will be set to nullptr.
	void GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
	                        bool requestHighPerformanceAdapter = false);

	// Specific initialization process.
	CEDX12WidgetPrivate* CreateViewport();
	CEDX12WidgetPrivate* LoadPSO();
	CEDX12WidgetPrivate* LoadAssets();

	// Specific render process.
	CEDX12WidgetPrivate* OnUpdate();
	CEDX12WidgetPrivate* OnRender();

	// Record all the commands we need to render the scene into the command list.
	CEDX12WidgetPrivate* PopulateCommandList();

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	CEDX12WidgetPrivate* WaitForPreviousFrame();

	// Viewport aspect ratio.
	float m_aspectRatio;

	bool m_useWarpDevice;

	// Root assets path.
	QString m_assetsPath;

	// Total frame number of swap chain.
	static const UINT FrameCount = 2;

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	// Pipeline objects.
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources.
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
};
