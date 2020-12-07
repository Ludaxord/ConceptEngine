#pragma once
#include "CEDirect3DGraphics.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

class CEDirect3D12Graphics : public CEDirect3DGraphics {

public:
	CEDirect3D12Graphics(HWND hWnd);
	~CEDirect3D12Graphics() = default;

public:
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;


public:
	struct CD3DX12_DEFAULT {
	};

	struct CD3DX12_ROOT_SIGNATURE_DESC : public D3D12_ROOT_SIGNATURE_DESC {
		CD3DX12_ROOT_SIGNATURE_DESC() = default;

		explicit
		CD3DX12_ROOT_SIGNATURE_DESC(const D3D12_ROOT_SIGNATURE_DESC& o) noexcept : D3D12_ROOT_SIGNATURE_DESC(o) {

		}

		CD3DX12_ROOT_SIGNATURE_DESC(UINT numParameters,
		                            _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
		                            UINT numStaticSamplers = 0,
		                            _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers
			                            = nullptr,
		                            D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {
			Init(numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
		}

		CD3DX12_ROOT_SIGNATURE_DESC(CD3DX12_DEFAULT) noexcept {
			Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);
		}

		inline void Init(UINT numParameters,
		                 _In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
		                 UINT numStaticSamplers = 0,
		                 _In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = nullptr,
		                 D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {
			Init(*this, numParameters, _pParameters, numStaticSamplers, _pStaticSamplers, flags);
		}

		static inline void Init(
			_Out_ D3D12_ROOT_SIGNATURE_DESC& desc,
			UINT numParameters,
			_In_reads_opt_(numParameters) const D3D12_ROOT_PARAMETER* _pParameters,
			UINT numStaticSamplers = 0,
			_In_reads_opt_(numStaticSamplers) const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = nullptr,
			D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) noexcept {
			desc.NumParameters = numParameters;
			desc.pParameters = _pParameters;
			desc.NumStaticSamplers = numStaticSamplers;
			desc.pStaticSamplers = _pStaticSamplers;
			desc.Flags = flags;
		}
	};

protected:
	void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
	                        bool requestHighPerformanceAdapter = false);

	void PopulateCommandList();
	void WaitForPreviousFrame();
	void LoadPipeline(HWND hWnd);
	void LoadAssets();

private:
	static const UINT FrameCount = 2;
protected:
	bool useWarpDevice = false;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	wrl::ComPtr<IDXGISwapChain3> pSwap;
	wrl::ComPtr<ID3D12Device> pDevice;
	wrl::ComPtr<ID3D12Resource> pTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> pCommandAllocator;
	wrl::ComPtr<ID3D12CommandQueue> pCommandQueue;
	wrl::ComPtr<ID3D12RootSignature> m_rootSignature;
	wrl::ComPtr<ID3D12DescriptorHeap> pRtvHeap;
	wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT pTargetViewDescriptorSize{};
	wrl::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};

	UINT pFrameIndex{};
	HANDLE m_fenceEvent{};
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue{};
};
