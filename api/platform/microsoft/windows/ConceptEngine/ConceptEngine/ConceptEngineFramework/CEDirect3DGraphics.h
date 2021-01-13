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
#include <wincodec.h>

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

	//TODO: create struct for color and texture coordinates
	struct CEVertexPosColor {
		XMFLOAT3 position;
		XMFLOAT4 color;
		// XMFLOAT2 texCoord;
	};

	struct CEVertexPosTex {
		XMFLOAT3 position;
		// XMFLOAT4 color;
		XMFLOAT2 texCoord;
	};

	struct CETextVertex {
		CETextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y,
		             float w, float h) : color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {
		}

		XMFLOAT4 pos;
		XMFLOAT4 texCoord;
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

	//TEST:
	struct Vertex {
		Vertex(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), color(r, g, b, z) {
		}

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};


	struct FontChar {
		// the unicode id
		int id;

		// these need to be converted to texture coordinates 
		// (where 0.0 is 0 and 1.0 is textureWidth of the font)
		float u; // u texture coordinate
		float v; // v texture coordinate
		float twidth; // width of character on texture
		float theight; // height of character on texture

		float width; // width of character in screen coords
		float height; // height of character in screen coords

		// these need to be normalized based on size of font
		float xoffset; // offset from current cursor pos to left side of character
		float yoffset; // offset from top of line to top of character
		float xadvance; // how far to move to right for next character
	};

	struct FontKerning {
		int firstid; // the first character
		int secondid; // the second character
		float amount; // the amount to add/subtract to second characters x
	};

	struct Font {
		std::wstring name; // name of the font
		std::wstring fontImage;
		int size; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
		float lineHeight; // how far to move down to next line, will be normalized
		float baseHeight; // height of all characters, will be normalized
		int textureWidth; // width of the font texture
		int textureHeight; // height of the font texture
		int numCharacters; // number of characters in the font
		FontChar* CharList; // list of characters
		int numKernings; // the number of kernings
		FontKerning* KerningsList; // list to hold kerning values
		wrl::ComPtr<ID3D12Resource> textureBuffer; // the font texture resource
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv

		// these are how much the character is padded in the texture. We
		// add padding to give sampling a little space so it does not accidentally
		// padd the surrounding characters. We will need to subtract these paddings
		// from the actual spacing between characters to remove the gaps you would otherwise see
		float leftpadding;
		float toppadding;
		float rightpadding;
		float bottompadding;

		// this will return the amount of kerning we need to use for two characters
		float GetKerning(wchar_t first, wchar_t second) {
			for (int i = 0; i < numKernings; ++i) {
				if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
					return KerningsList[i].amount;
			}
			return 0.0f;
		}

		// this will return a FontChar given a wide character
		FontChar* GetChar(wchar_t c) {
			for (int i = 0; i < numCharacters; ++i) {
				if (c == (wchar_t)CharList[i].id)
					return &CharList[i];
			}
			return nullptr;
		}
	};

	struct Timer {
		double timerFrequency = 0.0;
		long long lastFrameTime = 0;
		long long lastSecond = 0;
		double frameDelta = 0;
		int fps = 0;

		Timer() {
			LARGE_INTEGER li;
			QueryPerformanceFrequency(&li);

			// seconds
			//timerFrequency = double(li.QuadPart);

			// milliseconds
			timerFrequency = double(li.QuadPart) / 1000.0;

			// microseconds
			//timerFrequency = double(li.QuadPart) / 1000000.0;

			QueryPerformanceCounter(&li);
			lastFrameTime = li.QuadPart;
		}

		// Call this once per frame
		double GetFrameDelta() {
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			frameDelta = double(li.QuadPart - lastFrameTime) / timerFrequency;
			if (frameDelta > 0)
				fps = 1000 / frameDelta;
			lastFrameTime = li.QuadPart;
			return frameDelta;
		}
	};

	struct CEFence {
		ID3D12Fence* fence;
		UINT64 fenceValue;
	};

public:
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);
	CEDirect3DGraphics(const CEDirect3DGraphics&) = delete;
	CEDirect3DGraphics& operator=(const CEDirect3DGraphics&) = delete;
	~CEDirect3DGraphics() = default;

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
	bool CheckVSyncSupport() const;
	void WaitForPreviousFrame();
	void LoadPipeline();
	void LoadAssets();
	void Init();

	//TODO:Think about best solution ComPtr, shared_ptr or *
	//TODO:Also find global abstract for all DXGI elements with numbers
	IDXGIFactory4* CreateFactory() const;
	IDXGIAdapter1* CreateAdapter(IDXGIFactory4* dxgiFactory) const;
	//Implement later to dynamic change gpu.
	void GetAdaptersList();

	ID3D12Device* CreateDevice(IDXGIAdapter1* adapter);

	ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device);

	DXGI_SAMPLE_DESC CreateSampleDescriptor(int multiSampleCount = 1);

	IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* dxgiFactory,
	                                 ID3D12CommandQueue* commandQueue,
	                                 uint32_t screenWidth,
	                                 uint32_t screenHeight,
	                                 int bufferCount,
	                                 HWND outputWindow,
	                                 bool windowed,
	                                 DXGI_SAMPLE_DESC sampleDesc,
	                                 DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD);
	UINT GetFrameIndex(IDXGISwapChain3* swapChain);
	ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device,
	                                           int descriptorCount,
	                                           D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	                                           D3D12_DESCRIPTOR_HEAP_FLAGS heapFlags) const;
	UINT GetDescriptorSize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
	std::vector<ID3D12Resource*> CreateRenderTargetView(ID3D12DescriptorHeap* heap,
	                                                    int bufferCount,
	                                                    IDXGISwapChain3* swapChain,
	                                                    ID3D12Device* device,
	                                                    UINT descriptorSize,
	                                                    UINT offsetInDescriptor = 1) const;
	std::vector<ID3D12CommandAllocator*> CreateCommandAllocators(ID3D12Device* device, int bufferCount) const;
	ID3D12GraphicsCommandList* CreateCommandList(ID3D12Device* device,
	                                             std::vector<ID3D12CommandAllocator*> commandAllocators,
	                                             UINT frameIndex) const;
	std::vector<CEFence> CreateFence(ID3D12Device* device, int bufferCount) const;
	HANDLE CreateFenceEvent() const;
	D3D12_ROOT_DESCRIPTOR CreateRootDescriptor();
	std::vector<D3D12_DESCRIPTOR_RANGE> CreateDescriptorRange(int range, int descriptorNumber = 1,
	                                                          int shaderRegister = 0);
	D3D12_ROOT_DESCRIPTOR_TABLE CreateDescriptorTable(std::vector<D3D12_DESCRIPTOR_RANGE> descriptorTableRanges);
	std::vector<D3D12_ROOT_PARAMETER> CreateRootParameters(
		D3D12_ROOT_DESCRIPTOR rootDescriptor, D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable);
	D3D12_STATIC_SAMPLER_DESC CreateStaticSampler(int shaderRegister = 0,
	                                              int registerSpace = 0,
	                                              float minLevelOfDetail = 0.0f,
	                                              float maxLevelOfDetail = D3D12_FLOAT32_MAX);
	ID3D12RootSignature* CreateRootSignature(ID3D12Device* device, std::vector<D3D12_ROOT_PARAMETER> rootParameters,
	                                         int staticSampler, D3D12_STATIC_SAMPLER_DESC sampler) const;
	ID3DBlob* CompileShaderFromFile(std::wstring shaderFilePath, const char* entryPoint,
	                                const char* shaderCompileTarget);
	D3D12_SHADER_BYTECODE CreateShaderByteCode(ID3DBlob* shader);
	D3D12_BLEND_DESC CreateBlendDescriptor();
	ID3D12PipelineState* CreatePipelineState(ID3D12Device* device,
	                                         ID3D12RootSignature* rootSignature,
	                                         std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout,
	                                         D3D12_SHADER_BYTECODE vertexShaderByteCode,
	                                         D3D12_SHADER_BYTECODE pixelShaderByteCode,
	                                         DXGI_SAMPLE_DESC sampleDesc,
	                                         D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(
		                                         D3D12_DEFAULT),
	                                         D3D12_BLEND_DESC blendStateDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
	                                         D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(
		                                         D3D12_DEFAULT)) const;
	ID3D12Resource* CreateHeap(ID3D12Device* device,
	                           D3D12_RESOURCE_DESC* resourceDesc,
	                           D3D12_HEAP_TYPE heapType,
	                           D3D12_RESOURCE_STATES resourceState,
	                           D3D12_CLEAR_VALUE* clearValue, std::wstring heapName = L"Resource Heap") const;
	UINT64 GetUploadBufferSize(ID3D12Device* device, D3D12_RESOURCE_DESC desc);
	void CopyToDefaultHeap(ID3D12GraphicsCommandList* commandList, ID3D12Resource* destinationHeap,
	                       ID3D12Resource* intermediateHeap,
	                       const BYTE* dataArray, int bufferSize, bool useSlicePitch = false, int slicePitch = 0) const;
	void TransitionToBuffer(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
	                        D3D12_RESOURCE_STATES stateBefore,
	                        D3D12_RESOURCE_STATES stateAfter,
	                        int barrierNumber = 1);

	void CreateDepthStencilView(ID3D12Device* device, ID3D12DescriptorHeap* heap, ID3D12Resource* buffer,
	                            D3D12_DEPTH_STENCIL_VIEW_DESC* depthStencilDesc);
	std::tuple<int, D3D12_RESOURCE_DESC, BYTE*> LoadTexture(std::wstring texturePath);
	void CreateShaderResourceView(ID3D12Device* device,
	                              ID3D12Resource* buffer,
	                              ID3D12DescriptorHeap* heap,
	                              D3D12_RESOURCE_DESC textureDesc,
	                              D3D12_SRV_DIMENSION viewDimension,
	                              int mipLevels = 1,
	                              D3D12_CPU_DESCRIPTOR_HANDLE handle = {0}) const;
	std::tuple<int, D3D12_RESOURCE_DESC, BYTE*> LoadFonts(std::wstring fontPath, int width, int height);
	void InitCommandList(ID3D12GraphicsCommandList* commandList, ID3D12CommandQueue* commandQueue);
	void IncrementFenceValue(ID3D12CommandQueue* commandQueue, std::vector<CEFence> fences, UINT frameIndex);
	D3D12_VERTEX_BUFFER_VIEW CreateVertexBufferView(ID3D12Resource* buffer, UINT stride, UINT size);
	D3D12_INDEX_BUFFER_VIEW CreateIndexBufferView(ID3D12Resource* buffer, DXGI_FORMAT indexBufferFormat, UINT size);
	CD3DX12_VIEWPORT CreateViewPort(int topLeftX, int topLeftY, int width, int height, float minDepth,
	                                float maxDepth) const;
	CD3DX12_RECT CreateScissorRect(int left, int right, int top, int bottom) const;

	XMFLOAT4X4 BuildProjection(float angleY, int width, int height, float nearZ, float farZ);
	XMFLOAT4X4 CreateViewMatrix(XMFLOAT4 position, XMFLOAT4 target, XMFLOAT4 up);
	XMFLOAT4X4 CreateTranslationMatrix(XMFLOAT4 position, XMFLOAT4 offset = { 0, 0, 0, 0 });

	void ResetCommandAllocators(std::vector<ID3D12CommandAllocator*> commandAllocators, UINT frameIndex);
	void ResetCommandList(ID3D12GraphicsCommandList* commandList,
	                      std::vector<ID3D12CommandAllocator*> commandAllocators,
	                      UINT frameIndex,
	                      ID3D12PipelineState* pipelineState);
	void SetRenderTarget(ID3D12GraphicsCommandList* commandList,
	                     D3D12_CPU_DESCRIPTOR_HANDLE* renderTargetViewHandle,
	                     CD3DX12_CPU_DESCRIPTOR_HANDLE* depthStencilViewHandle,
	                     int renderTargetDescriptorsCount);
	void ClearRenderTargetView(ID3D12GraphicsCommandList* commandList,
	                           D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle, float clearColor[4],
	                           int rectNumber = 0) const;
	void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList,
	                           ID3D12DescriptorHeap* depthStencilViewHeap,
	                           float depth,
	                           float stencil,
	                           int rectNumber) const;
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList* commandList,
	                              ID3D12RootSignature* rootSignature) const;
	void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList,
	                        std::vector<ID3D12DescriptorHeap*> descriptorHeaps);
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* descriptorHeap,
	                                    UINT rootParameterIndex);
	void SetViewports(ID3D12GraphicsCommandList* commandList, D3D12_VIEWPORT* viewPort,
	                  UINT viewPortsNumber);
	void SetScissorRects(ID3D12GraphicsCommandList* commandList, D3D12_RECT* scissorRect,
	                     UINT scissorRectsNumber);
	void SetBuffers(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW* vertexBufferView,
	                D3D12_INDEX_BUFFER_VIEW* indexBufferView, UINT vertexBufferViewsNumber,
	                UINT vertexBufferViewStart = 0);
	void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* commandList,
	                                       ID3D12Resource* constantBufferUploadHeap, UINT rootParameterIndex);
	void DrawObject(ID3D12GraphicsCommandList* commandList,
	                UINT cubeIndices,
	                int instanceCount = 1,
	                int startIndexLocation = 0,
	                int baseVertexLocation = 0,
	                int startInstanceLocation = 0);
	void CloseCommandList(ID3D12GraphicsCommandList* commandList);

	bool InitD3D12();

public:
	void LoadBonus() override;

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
	DXGI_ADAPTER_DESC GetAdapterDescription(wrl::ComPtr<IDXGIAdapter> dxgiAdapter) const;

	void UpdatePipeline();

	void UpdateMultiplierColors();
	void UpdateCubesRotation();

	int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename,
	                          int& bytesPerRow);
	DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
	WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);

	Font LoadFont(LPCWSTR filename, int windowWidth, int windowHeight); // load a font

	void RenderText(Font font, std::wstring text, XMFLOAT2 pos, XMFLOAT2 scale = XMFLOAT2(1.0f, 1.0f),
	                XMFLOAT2 padding = XMFLOAT2(0.5f, 0.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

private:
	// Pipeline objects.
	DXGI_ADAPTER_DESC adapterDescription_;
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<ID3D12Device> m_device;
	wrl::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
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

	struct ConstantBuffer {
		XMFLOAT4 colorMultiplier;
	};

	// wrl::ComPtr<ID3D12DescriptorHeap> mainDescriptorHeap[FrameCount];
	// this heap will store the descripor to our constant buffer
	wrl::ComPtr<ID3D12Resource> constantBufferUploadHeap[FrameCount];
	// this is the memory on the gpu where our constant buffer will be placed.

	ConstantBuffer cbColorMultiplierData; // this is the constant buffer data we will send to the gpu 
	// (which will be placed in the resource we created above)

	UINT8* cbColorMultiplierGPUAddress[FrameCount];

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

	wrl::ComPtr<ID3D12Fence> m_fences[FrameCount];
	// an object that is locked while our command list is being executed by the gpu. We need as many 
	//as we have allocators (more if we want to know when the gpu is finished with an asset)
	UINT64 m_fenceValues[FrameCount]; // this value is incremented each frame. each fence will have its own value


	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
	bool m_ContentLoaded;

	// Window rectangle (used to toggle fullscreen state).
	RECT g_WindowRect;

	// this is the structure of our constant buffer.
	struct ConstantBufferPerObject {
		XMFLOAT4X4 wvpMat;
	};

	// Constant buffers must be 256-byte aligned which has to do with constant reads on the GPU.
	// We are only able to read at 256 byte intervals from the start of a resource heap, so we will
	// make sure that we add padding between the two constant buffers in the heap (one for cube1 and one for cube2)
	// Another way to do this would be to add a float array in the constant buffer structure for padding. In this case
	// we would need to add a float padding[50]; after the wvpMat variable. This would align our structure to 256 bytes (4 bytes per float)
	// The reason i didn't go with this way, was because there would actually be wasted cpu cycles when memcpy our constant
	// buffer data to the gpu virtual address. currently we memcpy the size of our structure, which is 16 bytes here, but if we
	// were to add the padding array, we would memcpy 64 bytes if we memcpy the size of our structure, which is 50 wasted bytes
	// being copied.
	int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;

	ConstantBufferPerObject cbPerObject; // this is the constant buffer data we will send to the gpu 
	// (which will be placed in the resource we created above)

	wrl::ComPtr<ID3D12Resource> constantBufferUploadHeaps[FrameCount];
	// this is the memory on the gpu where constant buffers for each frame will be placed

	UINT8* cbvGPUAddress[FrameCount]; // this is a pointer to each of the constant buffer resource heaps

	XMFLOAT4X4 cameraProjMat; // this will store our projection matrix
	XMFLOAT4X4 cameraViewMat; // this will store our view matrix

	XMFLOAT4 cameraPosition; // this is our cameras position vector
	XMFLOAT4 cameraTarget; // a vector describing the point in space our camera is looking at
	XMFLOAT4 cameraUp; // the worlds up vector

	XMFLOAT4X4 cube1WorldMat; // our first cubes world matrix (transformation matrix)
	XMFLOAT4X4 cube1RotMat; // this will keep track of our rotation for the first cube
	XMFLOAT4 cube1Position; // our first cubes position in space

	XMFLOAT4X4 cube2WorldMat; // our first cubes world matrix (transformation matrix)
	XMFLOAT4X4 cube2RotMat; // this will keep track of our rotation for the second cube
	XMFLOAT4 cube2PositionOffset;
	// our second cube will rotate around the first cube, so this is the position offset from the first cube

	int numCubeIndices; // the number of indices to draw the cube

	wrl::ComPtr<ID3D12Resource> textureBuffer; // the resource heap containing our texture
	wrl::ComPtr<ID3D12Resource> textureBuffer1; // the resource heap containing our texture

	wrl::ComPtr<ID3D12DescriptorHeap> mainDescriptorHeap;
	wrl::ComPtr<ID3D12Resource> textureBufferUploadHeap;

	BYTE* imageData;

	std::wstring debugImagePath;

	bool Running = true;

	UINT srvHandleSize;

	ID3D12PipelineState* textPSO; // pso containing a pipeline state

	Font mrRobotFont; // this will store our mr-robot font information

	int maxNumTextCharacters = 1024;
	// the maximum number of characters you can render during a frame. This is just used to make sure
	// there is enough memory allocated for the text vertex buffer each frame

	wrl::ComPtr<ID3D12Resource> textVertexBuffer[FrameCount];
	D3D12_VERTEX_BUFFER_VIEW textVertexBufferView[FrameCount]; // a view for our text vertex buffer
	UINT8* textVBGPUAddress[FrameCount]; // this is a pointer to each of the text constant buffers

	// create an instance of timer
	Timer timer;

};
