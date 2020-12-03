#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

#include "CEDirect3DGraphics.h"
#include "CEGraphics.h"


class CEDirect3D12Graphics : public CEDirect3DGraphics {

public:
	CEDirect3D12Graphics(HWND hWnd);
	~CEDirect3D12Graphics() = default;

public:
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;

private:
	static const UINT FrameCount = 2;
protected:
	bool useWarpDevice = false;

	wrl::ComPtr<IDXGISwapChain3> pSwap;
	wrl::ComPtr<ID3D12Device> pDevice;
	wrl::ComPtr<ID3D12Resource> pTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
	wrl::ComPtr<ID3D12RootSignature> m_rootSignature;
	wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;
	wrl::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
};
