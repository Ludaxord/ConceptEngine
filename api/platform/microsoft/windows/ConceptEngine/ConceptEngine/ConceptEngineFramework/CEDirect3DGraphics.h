#pragma once
#include "CEGraphics.h"
#include "CEInfoManager.h"

#include <d3d11.h>
#include "CEDirect3D11Manager.h"
#include "CEOSTools.h"

#pragma comment(lib, "d3d11.lib")

// #include <d3d12.h>
// #include <dxgi1_6.h>
// #include <DirectXMath.h>
//
// #include "CED3D12Helper.h"

// #pragma comment(lib, "d3d12.lib")
// #pragma comment(lib, "dxgi.lib")
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
	// explicit CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType);
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEDirect3DGraphics(const CEDirect3DGraphics&) = delete;
	CEDirect3DGraphics& operator=(const CEDirect3DGraphics&) = delete;
	~CEDirect3DGraphics();

public:
	CEGraphicsManager GetGraphicsManager() override;
	void EnableDebugLayer();

private:
	void CreateDirect3D12(int width, int height);
	void CreateDirect3D11(int width, int height);
public:
	void PrintGraphicsVersion() override;
protected:
#ifndef NDEBUG
	CEInfoManager infoManager;
#endif
private:
	CEDirect3D11Manager pManager;

public:
	wrl::ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp) const;
	wrl::ComPtr<ID3D12Device2> CreateDevice(wrl::ComPtr<IDXGIAdapter4> adapter) const;
	wrl::ComPtr<ID3D12CommandQueue> CreateCommandQueue(wrl::ComPtr<ID3D12Device2> device,
	                                                   D3D12_COMMAND_LIST_TYPE type) const;
	wrl::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd, wrl::ComPtr<ID3D12CommandQueue> commandQueue,
	                                             uint32_t width,
	                                             uint32_t height, uint32_t bufferCount);
	wrl::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(wrl::ComPtr<ID3D12Device2> device,
	                                                       D3D12_DESCRIPTOR_HEAP_TYPE type,
	                                                       uint32_t numDescriptors) const;
	void UpdateRenderTargetViews(wrl::ComPtr<ID3D12Device2> device, wrl::ComPtr<IDXGISwapChain4> swapChain,
	                             wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap);
	wrl::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(wrl::ComPtr<ID3D12Device2> device,
	                                                           D3D12_COMMAND_LIST_TYPE type);
	wrl::ComPtr<ID3D12GraphicsCommandList> CreateCommandList(wrl::ComPtr<ID3D12Device2> device,
	                                                         wrl::ComPtr<ID3D12CommandAllocator> commandAllocator,
	                                                         D3D12_COMMAND_LIST_TYPE type);
	wrl
	::ComPtr<ID3D12Fence> CreateFence(wrl::ComPtr<ID3D12Device2> device);
	HANDLE CreateEventHandle();
	uint64_t Signal(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	                uint64_t& fenceValue);
	void WaitForFenceValue(wrl::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
	                       std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	           uint64_t& fenceValue, HANDLE fenceEvent);
	bool CheckVSyncSupport() const;

	void WaitForPreviousFrame();

public:
	void OnUpdate() override;
	void OnRender() override;
	void Resize(uint32_t width, uint32_t height);
	void SetFullscreen(bool fullscreen) override;
	void OnInit() override;
	void OnDestroy() override;

private:
	//Graphics variables
	const uint8_t g_NumFrames = 3;
	bool g_UseWarp = false;
	uint32_t g_ClientWidth = 1280;
	uint32_t g_ClientHeight = 720;

	// Window rectangle (used to toggle fullscreen state).
	RECT g_WindowRect;

	// DirectX 12 Objects
	wrl::ComPtr<ID3D12Device2> g_Device;
	wrl::ComPtr<ID3D12CommandQueue> g_CommandQueue;
	wrl::ComPtr<IDXGISwapChain4> g_SwapChain;
	wrl::ComPtr<ID3D12Resource> g_BackBuffers[3];
	wrl::ComPtr<ID3D12GraphicsCommandList> g_CommandList;
	wrl::ComPtr<ID3D12CommandAllocator> g_CommandAllocators[3];
	wrl::ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
	UINT g_RTVDescriptorSize;
	UINT g_CurrentBackBufferIndex;

	// CPU && GPU Synchronization objects it is obligatory because
	// CPU is faster at issuing commands than the command queue is at processing those commands,
	// the CPU will have to stall at some point in order to allow the command queue to catch - up to the CPU.
	wrl::ComPtr<ID3D12Fence> g_Fence;
	uint64_t g_FenceValue = 0;
	uint64_t g_FrameFenceValues[3] = {};
	HANDLE g_FenceEvent;

	//Swap Chain Present Methods
	// By default, enable V-Sync.
	// Can be toggled with the V key.
	bool g_VSync = true;
	bool g_TearingSupported = false;
	// By default, use windowed mode.
	// Can be toggled with the Alt+Enter or F11
	bool g_Fullscreen = false;
};
