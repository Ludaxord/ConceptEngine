#pragma once
#include "CEGraphics.h"
#include "CEInfoManager.h"

#include <d3d11.h>
#include "CEDirect3D11Manager.h"
#include "CEOSTools.h"

#pragma comment(lib, "d3d11.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


using namespace DirectX;

class CEDirect3DGraphics : public CEGraphics {
public:
	// explicit CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType);
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);

	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;
	CEGraphicsManager GetGraphicsManager() override;

private:
	void CreateDirect3D12(HWND hWnd, int width, int height);
	void CreateDirect3D11(HWND hWnd, int width, int height);
public:
	void PrintGraphicsVersion() override;
protected:
#ifndef NDEBUG
	CEInfoManager infoManager;
#endif
private:
	CEDirect3D11Manager pManager;


	//TODO: REFACTOR ALL OF THIS, right now is just to enable direct3D 12
protected:
	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);
private:
	static const UINT FrameCount = 2;

	struct CEVertex {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	// Pipeline objects.
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<ID3D12Device> m_device;
	wrl::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
	wrl::ComPtr<ID3D12RootSignature> m_rootSignature;
	wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources.
	wrl::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	bool m_useWarpDevice;

};
