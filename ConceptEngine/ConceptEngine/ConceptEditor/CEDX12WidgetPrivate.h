#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include "CEDX12Widget.h"
#include <private/qwidget_p.h>
#include <wrl/client.h>

class CEDX12WidgetPrivate final : public QWidgetPrivate {
	Q_DECLARE_PUBLIC(CEDX12Widget)

public:
	explicit CEDX12WidgetPrivate(CEDX12Widget* parent);
	~CEDX12WidgetPrivate();

private:
	// Init DirectX12 3D.
	void initialize();

	// DirectX12 3D Render.
	void render();

	// Handle viewport size when window size change.
	void resize();

	// Helper function for resolving the full path of assets.
	QString getAssetFullPath(const QString& assetName) const { return m_assetsPath + assetName; }

	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	// If no such adapter can be found, *ppAdapter will be set to nullptr.
	void getHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
	                        bool requestHighPerformanceAdapter = false);

	// Specific initialization process.
	CEDX12WidgetPrivate* initViewport();
	CEDX12WidgetPrivate* loadPipeline();
	CEDX12WidgetPrivate* loadAssets();

	// Specific render process.
	CEDX12WidgetPrivate* onUpdate();
	CEDX12WidgetPrivate* onRender();

	// Record all the commands we need to render the scene into the command list.
	CEDX12WidgetPrivate* populateCommandList();

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	CEDX12WidgetPrivate* waitForPreviousFrame();

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
