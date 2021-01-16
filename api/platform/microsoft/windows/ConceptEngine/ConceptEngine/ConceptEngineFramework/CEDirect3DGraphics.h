#pragma once
#include "CEGraphics.h"

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


#include "CED3D12UploadBuffer.h"
#include "CEMath.h"
#include "d3dx12.h"
#include <DirectXColors.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

using namespace DirectX;
namespace fs = std::filesystem;


struct CEObjectConstants {
	XMFLOAT4X4 WorldViewProjection = CEMath::Identity4x4();
};

class CEDirect3DGraphics : public CEGraphics {

public:

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

	struct CVertex {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
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

	bool CheckVSyncSupport() const;
	void WaitForPreviousFrame();
	void UpdateCubes();
	void RenderCubes();
	bool InitCubes();

protected:
	void InitGui() override;
	void RenderGui() override;
	void DestroyGui() override;

	void InitImGui();
	void RenderImGui() const;
	void DestroyImGui() const;

public:
	void LogSystemInfo() override;

	void OnUpdate() override;
	void OnRender() override;
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


private:
	/*
	 * Direct3D Initialization
	 */
	void InitDirectX();
	DXGI_ADAPTER_DESC GetAdapterDescription(IDXGIAdapter* dxgiAdapter) const;
	IDXGIFactory4* CreateFactory() const;
	std::tuple<ID3D12Device*, IDXGIAdapter1*> CreateDeviceAndAdapter(IDXGIFactory4* factory,
	                                                                D3D_FEATURE_LEVEL featureLevel =
		                                                                D3D_FEATURE_LEVEL_11_0);
	ID3D12Fence* CreateFence(ID3D12Device* device, int initialValue = 0,
	                         D3D12_FENCE_FLAGS fenceFlags = D3D12_FENCE_FLAG_NONE) const;
	CEOSTools::CESystemInfo CEDirect3DGraphics::GetSystemInfo(IDXGIAdapter1* adapter) const;
	UINT GetDescriptorHandleIncrementSize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType);
	UINT Check4XMSAAQualitySupport(ID3D12Device* device, DXGI_FORMAT backBufferFormat, int sampleCount = 4,
	                               D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS multisampleFlags =
		                               D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, int qualityLevels = 0);
	std::tuple<ID3D12CommandQueue*, ID3D12CommandAllocator*, ID3D12GraphicsCommandList*> CreateCommandObjects(
		ID3D12Device* device);
	IDXGISwapChain* CreateSwapChain(HWND hWnd, IDXGIFactory4* factory,
	                                ID3D12CommandQueue* commandQueue,
	                                int clientWidth, int clientHeight, DXGI_FORMAT backBufferFormat,
	                                bool msaaState, UINT msaaQuality, UINT swapChainBufferCount);
	std::tuple<ID3D12DescriptorHeap*, ID3D12DescriptorHeap*> CreateDescriptorHeaps(
		ID3D12Device* device, UINT bufferCount) const;
	ID3D12DescriptorHeap* BuildDescriptorHeaps(ID3D12Device* device) const;
	void BuildConstantBuffers(ID3D12Device* device, ID3D12DescriptorHeap* constantBufferHeap, UINT elementsCount = 1);
	ID3D12RootSignature* BuildRootSignature(ID3D12Device* device);
	std::tuple<ID3DBlob*, ID3DBlob*, std::vector<D3D12_INPUT_ELEMENT_DESC>>
	BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();
	void FlushCommandQueue();
	void Draw();
	void Update();
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	void Resize();
	void ResizeProjection();
	float AspectRatio() const;
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	/*
	 * Cube Initialization Tools;
	 */
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


	/*
	 * Global variables;
	 */
private:
	DXGI_ADAPTER_DESC adapterDescription_;
	// create an instance of timer
	Timer timer;
	std::unique_ptr<CEGUI> m_Gui;
	CEOSTools::CESystemInfo systemInfo_;
	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;

	XMFLOAT4X4 mWorld = CEMath::Identity4x4();
	XMFLOAT4X4 mView = CEMath::Identity4x4();
	XMFLOAT4X4 mProj = CEMath::Identity4x4();
	static const int SwapChainBufferCount = 2;

	/*
	 * Variables to create cubes;
	 */
private:
	wrl::ComPtr<IDXGISwapChain> swapChain_;
	wrl::ComPtr<IDXGISwapChain3> m_swapChain;
	wrl::ComPtr<ID3D12Device> m_device;
	wrl::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
	wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
	wrl::ComPtr<ID3D12PipelineState> m_pipelineState;
	wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	wrl::ComPtr<IDXGIFactory4> m_factory;

	UINT m_rtvDescriptorSize;
	UINT m_dsvDescriptorSize;
	UINT m_cbvSrvUavDescriptorSize;
	bool m_useWarpDevice = false;

	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	UINT64 mCurrentFence = 0;
	int mCurrBackBuffer = 0;
	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	wrl::ComPtr<ID3D12PipelineState> mPSO = nullptr;
	std::unique_ptr<CED3D12UploadBuffer<CEObjectConstants>> m_constantBuffers = nullptr;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;


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
	// Root signature
	wrl::ComPtr<ID3D12RootSignature> m_RootSignature;

	struct ConstantBuffer {
		XMFLOAT4 colorMultiplier;
	};

	wrl::ComPtr<ID3DBlob> mvsByteCode = nullptr;
	wrl::ComPtr<ID3DBlob> mpsByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// wrl::ComPtr<ID3D12DescriptorHeap> mainDescriptorHeap[FrameCount];
	// this heap will store the descripor to our constant buffer
	wrl::ComPtr<ID3D12Resource> constantBufferUploadHeap[FrameCount];
	// this is the memory on the gpu where our constant buffer will be placed.

	ConstantBuffer cbColorMultiplierData; // this is the constant buffer data we will send to the gpu 
	// (which will be placed in the resource we created above)

	UINT8* cbColorMultiplierGPUAddress[FrameCount];

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

	wrl::ComPtr<ID3D12Resource> textureBufferUploadHeap;

	wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	// Descriptor heap for depth buffer.
	wrl::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	wrl::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;


	wrl::ComPtr<ID3D12DescriptorHeap> mainDescriptorHeap;
	wrl::ComPtr<ID3D12DescriptorHeap> g_pd3dSrvDescHeap;

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

};
