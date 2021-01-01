#pragma once
#include "CEGraphics.h"
#include "CEDirect3DInfoManager.h"

#include <d3d11.h>
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

#include "CED3D12CommandQueue.h"
#include "d3dx12.h"

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
	struct ID3DGPUInfo : IGPUInfo {
		DXGI_QUERY_VIDEO_MEMORY_INFO dqvmi;
		ID3DGPUInfo() = default;
		// conversion from A (constructor):
		explicit ID3DGPUInfo(const IGPUInfo& x) {
		}
	};

	struct CEVertexPosColor {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct CEVertexPositionColor : CEVertex {
		XMFLOAT3 Position = {pos.x, pos.y, pos.z};
		XMFLOAT3 Color;

		CEVertexPositionColor() = default;
		// conversion from A (constructor):
		CEVertexPositionColor(const CEVertex& x) {
			Position = {x.pos.x, x.pos.y, x.pos.y};
			Color = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}

		CEVertexPositionColor(const CEVertex& x, XMFLOAT3 col) {
			Position = {x.pos.x, x.pos.y, x.pos.y};
			Color = col;
		}

		CEVertexPositionColor(XMFLOAT3 pos, XMFLOAT3 col) {
			Position = pos;
			Color = col;
		}

		// conversion from A (assignment):
		CEVertexPositionColor& operator=(const CEVertex& x) { return *this; }
		// conversion to A (type-cast operator)
		operator CEVertexPositionColor() const { return CEVertex(); }
	};

	struct CED3DVertexBuffer : CEVertexBuffer<WORD> {
		using indexObject = CEIndex<WORD>;
		CED3DVertexBuffer() = default;
		// conversion from A (constructor):
		CED3DVertexBuffer(const CEVertexBuffer<WORD>& x) {
			this->indices = x.indices;
			this->vertices = x.vertices;
		}

		CED3DVertexBuffer(WORD* indicies, CEVertexPositionColor* vertexPosColor) {
			CreateVertices(vertexPosColor);
			CreateIndicies(indicies);
		}

		// conversion from A (assignment):
		CED3DVertexBuffer& operator=(const CEVertexBuffer<WORD>& x) { return *this; }
		// conversion to A (type-cast operator)
		operator CED3DVertexBuffer() const { return CEVertexBuffer<WORD>(); }

		void CreateIndicies(WORD* indicies) {
			this->indices = {};
			for (auto i = 0; i < sizeof(indicies); i++)
				this->indices.push_back(indexObject(indicies[i]));
			// this->indices[i] = indexObject(indicies[i]);
		}

		void CreateVertices(CEVertexPositionColor* vertexPosColor) {
			this->vertices = {};
			for (auto i = 0; i < sizeof(vertexPosColor); i++)
				this->vertices.push_back(vertexPosColor[i]);
			// this->vertices[i] = static_cast<CEVertex>(vertexPosColor[i]);
		}
	};

public:
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEDirect3DGraphics(const CEDirect3DGraphics&) = delete;
	CEDirect3DGraphics& operator=(const CEDirect3DGraphics&) = delete;
	~CEDirect3DGraphics() = default;

public:
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

public:
	void OnUpdate() override;
	void OnRender() override;
	void Resize(uint32_t width, uint32_t height);
	void SetFullscreen(bool fullscreen) override;
	bool OnInit() override;
	void OnDestroy() override;

	void UpdatePerSecond(float second) override;
	bool LoadContent() override;
	void UnloadContent() override;
protected:
	void OnKeyPressed() override;
	void OnKeyReleased() override;
	void OnMouseMoved() override;
	void OnMouseButtonPressed() override;
	void OnMouseButtonReleased() override;
	void OnMouseWheel() override;
	void OnResize() override;
	void OnWindowDestroy() override;
	IGPUInfo GetGPUInfo() override;

private:
	void DisplayGPUInfo();
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
	                                                         wrl::ComPtr<ID3D12CommandAllocator> commandAllocator,
	                                                         wrl::ComPtr<ID3D12PipelineState> pipelineState,
	                                                         D3D12_COMMAND_LIST_TYPE type);

	void CreateRootSignature();
	void CreateVertexBuffer();
	wrl::ComPtr<ID3D12Fence> CreateFence(wrl::ComPtr<ID3D12Device> device);
	HANDLE CreateEventHandle();
	uint64_t Signal(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	                uint64_t& fenceValue);
	void WaitForFenceValue(wrl::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
	                       std::chrono::milliseconds duration = std::chrono::milliseconds::max());
	void Flush(wrl::ComPtr<ID3D12CommandQueue> commandQueue, wrl::ComPtr<ID3D12Fence> fence,
	           uint64_t& fenceValue, HANDLE fenceEvent);

private:
	void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                        Microsoft::WRL::ComPtr<ID3D12Resource> resource,
	                        D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
	void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	              D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);
	void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f);
	void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	                          ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
	                          size_t numElements, size_t elementSize, const void* bufferData,
	                          D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
	void ResizeDepthBuffer(int width, int height);

private:
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

private:
	wrl::ComPtr<IDXGIAdapter> m_dxgiAdapter;
	// Vertex buffer for the cube.
	wrl::ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	// Index buffer for the cube.
	wrl::ComPtr<ID3D12Resource> m_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	// Depth buffer.
	wrl::ComPtr<ID3D12Resource> m_DepthBuffer;
	// Descriptor heap for depth buffer.
	wrl::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	// Root signature
	wrl::ComPtr<ID3D12RootSignature> m_RootSignature;

	std::shared_ptr<CED3D12CommandQueue> m_DirectCommandQueue;
	std::shared_ptr<CED3D12CommandQueue> m_ComputeCommandQueue;
	std::shared_ptr<CED3D12CommandQueue> m_CopyCommandQueue;

	CD3DX12_VIEWPORT m_Viewport;
	CD3DX12_RECT m_ScissorRect;
	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	wrl::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
	float m_FoV;

	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
	bool m_ContentLoaded;

	// Window rectangle (used to toggle fullscreen state).
	RECT g_WindowRect;
};
