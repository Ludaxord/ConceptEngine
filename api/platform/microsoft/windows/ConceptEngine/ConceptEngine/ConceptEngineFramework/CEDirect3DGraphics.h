#pragma once
#include "CEGraphics.h"
#include "CEDirect3DInfoManager.h"

#include <d3d11.h>
#include "CEDirect3D11Manager.h"
#include "CEOSTools.h"

#pragma comment(lib, "d3d11.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <filesystem>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

using namespace DirectX;
namespace fs = std::filesystem;


class CEDirect3DGraphics : public CEGraphics {
public:
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEDirect3DGraphics(const CEDirect3DGraphics&) = delete;
	CEDirect3DGraphics& operator=(const CEDirect3DGraphics&) = delete;
	~CEDirect3DGraphics() = default;

public:
	void SetGraphicsManager() override;
	void EnableDebugLayer();

private:
	void CreateDirect3D12(int width, int height);
	void CreateDirect3D11(int width, int height);
public:
	void PrintGraphicsVersion() override;
protected:
#ifndef NDEBUG
	CEDirect3DInfoManager infoManager;
#endif

private:
	void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
	                        bool requestHighPerformanceAdapter = false) const;
	void PopulateCommandList();
	bool CheckVSyncSupport() const;
	void WaitForPreviousFrame();
	void LoadPipeline();
	void LoadAssets();

private:
	wrl::ComPtr<IDXGIFactory4> GetFactory() const;
	wrl::ComPtr<IDXGIAdapter> GetAdapter(bool useWarp) const;
	wrl::ComPtr<ID3D12Device> CreateDevice(wrl::ComPtr<IDXGIAdapter> adapter) const;
	wrl::ComPtr<ID3D12CommandQueue> CreateCommandQueue(wrl::ComPtr<ID3D12Device> device,
	                                                   D3D12_COMMAND_LIST_TYPE type) const;
	wrl::ComPtr<IDXGISwapChain3> CreateSwapChain(
		HWND hWnd, wrl::ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width,
		uint32_t height, uint32_t bufferCount);
	wrl::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(wrl::ComPtr<ID3D12Device> device,
	                                                       D3D12_DESCRIPTOR_HEAP_TYPE type,
	                                                       uint32_t numDescriptors) const;
	void UpdateRenderTargetViews(wrl::ComPtr<ID3D12Device> device,
	                             wrl::ComPtr<IDXGISwapChain3> swapChain,
	                             wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap);
	wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(wrl::ComPtr<ID3D12Device> device,
	                                                           D3D12_COMMAND_LIST_TYPE type);
	wrl::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(wrl::ComPtr<ID3D12Device> device,
	                                                         wrl::ComPtr<ID3D12CommandAllocator>
	                                                         commandAllocator,
	                                                         D3D12_COMMAND_LIST_TYPE type);
	wrl::ComPtr<ID3D12Fence> CreateFence(wrl::ComPtr<ID3D12Device> device);
	HANDLE CreateEventHandle();
	uint64_t Signal(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	                uint64_t& fenceValue);
	void WaitForFenceValue(wrl::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
	                       std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	           uint64_t& fenceValue, HANDLE fenceEvent);

public:
	void OnUpdate() override;
	void OnRender() override;
	void Resize(uint32_t width, uint32_t height);
	void SetFullscreen(bool fullscreen) override;
	bool OnInit() override;
	void OnDestroy() override;

private:
	uint32_t g_ClientWidth = 1280;
	uint32_t g_ClientHeight = 720;
	static const UINT FrameCount = 2;

	// Pipeline objects.
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<ID3D12Device> m_device;
	wrl::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
	wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	wrl::ComPtr<IDXGIFactory4> m_factory;
	UINT m_rtvDescriptorSize;
	bool m_useWarpDevice = false;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	// Window rectangle (used to toggle fullscreen state).
	RECT g_WindowRect;

	//Swap Chain Present Methods
	// By default, enable V-Sync.
	// Can be toggled with the V key.
	bool g_VSync = true;
	bool g_TearingSupported = false;
	// By default, use windowed mode.
	// Can be toggled with the Alt+Enter or F11
	bool g_Fullscreen = false;
};
