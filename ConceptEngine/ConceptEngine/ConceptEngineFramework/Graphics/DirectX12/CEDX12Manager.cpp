#include "CEDX12Manager.h"


#include "../../Game/CEWindow.h"
#include "../../Tools/CEUtils.h"

#include <DirectXColors.h>
#include <fstream>


#include "CEDX12Playground.h"
#include "Libraries/DDSTextureLoader.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
namespace fs = std::filesystem;

void CEDX12Manager::Create() {
	EnableDebugLayer();
	CreateDXGIFactory();
	TearingSupport();
	CreateAdapter();
	CreateDevice();
	FeatureLevelSupport();
	DirectXRayTracingSupport();
	CreateFence();
	CreateDescriptorSizes();
	MultiSamplingSupport();

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandQueue();
	CreateCommandAllocator();
	CreateCommandList();
	CreateSwapChain();
	//TODO: Fix problem with dynamic creation of RTVDescriptor heap
	CreateRTVDescriptorHeap(
		BufferCount + 1
	);
	CreateDSVDescriptorHeap();

	LogDirectXInfo();

	Resize();

	m_playground->Create();
}

void CEDX12Manager::InitPlayground(CEPlayground* playground) {
	const std::string playgroundClassName = typeid(playground).name();
	spdlog::info("InitPlayground Loaded Class Name: {}", playgroundClassName);
	auto* const dx12Playground = reinterpret_cast<CEDX12Playground*>(playground);
	m_playground = dx12Playground;
	m_playground->Init(this);
}

void CEDX12Manager::Destroy() {
#ifdef _DEBUG
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
			                             DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}

void CEDX12Manager::Resize() {
	assert(m_d3dDevice);
	assert(m_swapChain);
	assert(m_commandAllocator);

	//Flush before changing resources
	FlushCommandQueue();

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	//Release previous resources that will be recreated
	for (int i = 0; i < BufferCount; ++i) {
		m_swapChainBuffer[i].Reset();
	}
	m_depthStencilBuffer.Reset();

	//Resize swap chain
	auto swapChainFlags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ThrowIfFailed(m_swapChain->ResizeBuffers(BufferCount,
	                                         m_window.GetWidth(),
	                                         m_window.GetHeight(),
	                                         m_backBufferFormat,
	                                         swapChainFlags));
	m_currentBackBuffer = 0;

	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	//Create render target buffer and view
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < BufferCount; i++) {
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));
		m_d3dDevice->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_descriptorSizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]);
	}

	//Create depth stencil buffer and view
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_window.GetWidth();
	depthStencilDesc.Height = m_window.GetHeight();
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = m_depthStencilFormat;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())
	));

	//Create descriptor to mip level 0 of entire resource using format of resource
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = m_depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	m_d3dDevice->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc,
	                                    m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//Transition resource from internal state to be used as depth buffer
	auto dsvTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandList->ResourceBarrier(1, &dsvTransitionBarrier);

	//Execute resize commands
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* commandLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until resize if complete
	FlushCommandQueue();

	//Update viewport transform to cover client area

	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width = static_cast<float>(m_window.GetWidth());
	m_screenViewport.Height = static_cast<float>(m_window.GetHeight());
	m_screenViewport.MinDepth = 0.0f;
	m_screenViewport.MaxDepth = 1.0f;

	m_scissorRect = {0, 0, m_window.GetWidth(), m_window.GetHeight()};

	m_playground->Resize();
}

bool CEDX12Manager::Initialized() {
	return m_d3dDevice;
}

void CEDX12Manager::Update(const CETimer& gt) {
	m_playground->Update(gt);
}

void CEDX12Manager::Render(const CETimer& gt) {
	m_playground->Render(gt);

}

Microsoft::WRL::ComPtr<IDXGIFactory4> CEDX12Manager::GetDXGIFactory() const {
	return m_dxgiFactory;
}

Microsoft::WRL::ComPtr<IDXGIAdapter1> CEDX12Manager::GetDXGIAdapter() const {
	return m_adapter;
}

Microsoft::WRL::ComPtr<ID3D12Device> CEDX12Manager::GetD3D12Device() const {
	return m_d3dDevice;
}

Microsoft::WRL::ComPtr<ID3D12Fence> CEDX12Manager::GetD3D12Fence() const {
	return m_fence;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CEDX12Manager::GetD3D12CommandQueue() const {
	return m_commandQueue;
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CEDX12Manager::GetD3D12CommandAllocator() const {
	return m_commandAllocator;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CEDX12Manager::GetD3D12CommandList() const {
	return m_commandList;
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> CEDX12Manager::GetDXGISwapChain() const {
	return m_swapChain;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDX12Manager::GetRTVDescriptorHeap() const {
	return m_rtvHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDX12Manager::GetDSVDescriptorHeap() const {
	return m_dsvHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDX12Manager::GetCBVDescriptorHeap() const {
	return m_cbvHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CEDX12Manager::GetSRVDescriptorHeap() const {
	return m_srvHeap;
}

UINT CEDX12Manager::GetCurrentBackBuffer() const {
	return m_currentBackBuffer;
}

UINT CEDX12Manager::GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) {
	return m_descriptorSizes[heapType];
}

D3D12_CPU_DESCRIPTOR_HANDLE CEDX12Manager::CurrentBackBufferView() {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_currentBackBuffer,
		GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE CEDX12Manager::DepthStencilView() const {
	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void CEDX12Manager::ExecuteCommandLists(std::vector<ID3D12CommandList*> commandLists) const {
	// Add the command list to the queue for execution.
	if (commandLists.size() == 0) {
		commandLists.push_back(m_commandList.Get());
	}

	m_commandQueue->ExecuteCommandLists(commandLists.size(), commandLists.data());
}

DXGI_FORMAT CEDX12Manager::GetBackBufferFormat() const {
	return m_backBufferFormat;
}

DXGI_FORMAT CEDX12Manager::GetDepthStencilFormat() const {
	return m_depthStencilFormat;
}

bool CEDX12Manager::GetVSync() const {
	return m_vSync;
}

bool CEDX12Manager::GetTearingSupport() const {
	return m_tearingSupported;
}

bool CEDX12Manager::GetRayTracingSupport() const {
	return m_rayTracingSupported;
}

bool CEDX12Manager::GetM4XMSAAState() const {
	return m_4xMsaaState;
}

bool CEDX12Manager::GetM4XMSAAQuality() const {
	return m_4xMsaaQuality;
}

bool CEDX12Manager::IsFullScreen() const {
	return m_window.IsFullScreen();
}

float CEDX12Manager::GetWindowWidth() const {
	return (float)m_window.GetWidth();
}

float CEDX12Manager::GetWindowHeight() const {
	return (float)m_window.GetHeight();
}

HWND CEDX12Manager::GetWindowHandle() const {
	return m_window.GetWindowHandle();
}

float CEDX12Manager::GetAspectRatio() const {
	return static_cast<float>(m_window.GetWidth()) / m_window.GetHeight();
}

int CEDX12Manager::GetCurrentBackBufferIndex() const {
	return m_currentBackBuffer;
}

void CEDX12Manager::SetCurrentBackBufferIndex(int backBufferIndex) {
	m_currentBackBuffer = backBufferIndex;
}

int CEDX12Manager::GetBackBufferCount() {
	return BufferCount;
}

D3D12_VIEWPORT CEDX12Manager::GetViewPort() const {
	return m_screenViewport;
}

void CEDX12Manager::SetViewPort(D3D12_VIEWPORT viewPort) {
	m_screenViewport = viewPort;
}

D3D12_RECT CEDX12Manager::GetScissorRect() const {
	return m_scissorRect;
}

void CEDX12Manager::SetScissorRect(D3D12_RECT scissorRect) {
	m_scissorRect = scissorRect;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CEDX12Manager::GetDepthStencilBuffer() const {
	return m_depthStencilBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Fence> CEDX12Manager::GetFence() const {
	return m_fence;
}

DirectX::XMVECTOR CEDX12Manager::SphericalToCartesian(float radius, float theta, float phi) {
	return DirectX::XMVectorSet(
		radius * sinf(phi) * cosf(theta),
		radius * cosf(phi),
		radius * sinf(phi) * sinf(theta),
		1.0f);
}

std::unique_ptr<Resources::Texture> CEDX12Manager::LoadTextureFromFile(const std::string fileName,
                                                                       std::string textureName) const {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream texturesPathStream;
	texturesPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Textures\\" <<
		fileName;
	auto texturesPath = texturesPathStream.str();
	spdlog::info("Loading Texture: {}", texturesPath);
	auto wTexturesPath = std::wstring(texturesPath.begin(), texturesPath.end());

	auto texture = std::make_unique<Resources::Texture>();
	texture->Name = textureName;
	texture->Filename = wTexturesPath;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(m_d3dDevice.Get(),
	                                                  m_commandList.Get(), texture->Filename.c_str(),
	                                                  texture->Resource, texture->UploadHeap));
	return texture;
}

Resources::CENode32 CEDX12Manager::LoadNodeFromTxt(const std::string fileName) {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		fileName;
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model from TXT: {}", modelPath);

	std::ifstream fin(modelPath);

	if (!fin) {
		spdlog::error("FILE NOT FOUND: {}", modelPath);
		return {};
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Resources::CENormalVertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	return {vertices, indices};
}

Resources::CENode CEDX12Manager::LoadNode(const std::string fileName) {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		fileName;
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model: {}", modelPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_GenBoundingBoxes);

	// If the import failed, report it
	if (!scene) {
		OutputDebugStringA("ERROR WHILE LOADING MODEL");
		OutputDebugStringA(importer.GetErrorString());
		spdlog::error(importer.GetErrorString());
	}

	int vcount = 0;
	int icount = 0;

	{
		// Import meshes
		for (unsigned int j = 0; j < scene->mNumMeshes; ++j) {
			auto* aiMesh = scene->mMeshes[j];
			unsigned int i;
			if (aiMesh->HasPositions()) {
				for (i = 0; i < aiMesh->mNumVertices; ++i) {
					vcount++;
				}
			}

			// Extract the index buffer.
			if (aiMesh->HasFaces()) {
				for (i = 0; i < aiMesh->mNumFaces; ++i) {
					const aiFace& face = aiMesh->mFaces[i];

					// Only extract triangular faces
					if (face.mNumIndices == 3) {
						icount++;
					}
				}
			}
		}
	}

	int vvcount = 0;
	int iicount = 0;

	std::vector<Resources::CENormalVertex> vertices(vcount);
	std::vector<std::uint64_t> indices(icount);
	std::vector<Resources::Material> materials(scene->mNumMaterials);

	for (unsigned int j = 0; j < scene->mNumMeshes; ++j) {
		auto* aiMesh = scene->mMeshes[j];
		unsigned int i;

		for (i = 0; i < aiMesh->mNumVertices; ++i) {
			if (aiMesh->HasPositions()) {
				vertices[vvcount].Pos = {
					aiMesh->mVertices[i].x,
					aiMesh->mVertices[i].y,
					aiMesh->mVertices[i].z
				};
			}
			if (aiMesh->HasNormals()) {
				vertices[vvcount].Normal = {
					aiMesh->mNormals[i].x,
					aiMesh->mNormals[i].y,
					aiMesh->mNormals[i].z
				};
			}
			vvcount++;
		}

		// Extract the index buffer.
		if (aiMesh->HasFaces()) {
			for (i = 0; i < aiMesh->mNumFaces; ++i) {
				const aiFace& face = aiMesh->mFaces[i];

				// Only extract triangular faces
				if (face.mNumIndices == 3) {
					indices.push_back(face.mIndices[0]);
					indices.push_back(face.mIndices[1]);
					indices.push_back(face.mIndices[2]);
					iicount++;
				}
			}
		}

		//TODO: implement in CEVertex Struct
		/*

		if (aiMesh->HasTangentsAndBitangents()) {
			for (i = 0; i < aiMesh->mNumVertices; ++i) {
				vertexData[i].Tangent = {aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z};
				vertexData[i].Bitangent = {
					aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z
				};
			}
		}

		if (aiMesh->HasTextureCoords(0)) {
			for (i = 0; i < aiMesh->mNumVertices; ++i) {
				vertexData[i].TexCoord = {
					aiMesh->mTextureCoords[0][i].x,
					aiMesh->mTextureCoords[0][i].y,
					aiMesh->mTextureCoords[0][i].z
				};
			}
		}
		 */
	}

	for (unsigned int j = 0; j < scene->mNumMaterials; ++j) {
		auto aiMaterial = scene->mMaterials[j];

		aiString materialName;
		aiString aiTexturePath;
		aiTextureOp aiBlendOperation;
		float blendFactor;
		aiColor4D diffuseColor;
		aiColor4D specularColor;
		aiColor4D ambientColor;
		aiColor4D emissiveColor;
		float opacity;
		float indexOfRefraction;
		float reflectivity;
		float shininess;
		float bumpIntensity;

		if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS) {
			auto XMAmbientColor = XMFLOAT4(ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a);
		}

		if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == aiReturn_SUCCESS) {
			auto XMEmissiveColor = XMFLOAT4(emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a);
		}

		if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS) {
			auto XMDiffuseColor = XMFLOAT4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
		}

		if (aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS) {
			auto XMSpecularColor = XMFLOAT4(specularColor.r, specularColor.g, specularColor.b, specularColor.a);
		}

		if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
			auto XMShininess = shininess;
		}

		if (aiMaterial->Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
			auto XMOpacity = opacity;
		}

		if (aiMaterial->Get(AI_MATKEY_REFRACTI, indexOfRefraction) == aiReturn_SUCCESS) {
			auto XMIndexOfRefraction = indexOfRefraction;
		}

		if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS) {
			auto XMReflectivity = XMFLOAT4(reflectivity, reflectivity, reflectivity, reflectivity);
		}

		if (aiMaterial->Get(AI_MATKEY_BUMPSCALING, bumpIntensity) == aiReturn_SUCCESS) {
			auto XMBumpIntensity = bumpIntensity;
		}
	}

	return {vertices, indices};
}

std::vector<Resources::CENode> CEDX12Manager::LoadNodes(const std::string fileName) {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		fileName;
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model: {}", modelPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_GenBoundingBoxes);

	// If the import failed, report it
	if (!scene) {
		OutputDebugStringA("ERROR WHILE LOADING MODEL");
		OutputDebugStringA(importer.GetErrorString());
		spdlog::error(importer.GetErrorString());
	}
	std::vector<Resources::CENode> nodes;

	for (unsigned int j = 0; j < scene->mNumMeshes; ++j) {
		auto* aiMesh = scene->mMeshes[j];
		unsigned int i;
		std::vector<Resources::CENormalVertex> vertices(aiMesh->mNumVertices);

		for (i = 0; i < aiMesh->mNumVertices; ++i) {
			if (aiMesh->HasPositions()) {
				vertices[i].Pos = {
					aiMesh->mVertices[i].x,
					aiMesh->mVertices[i].y,
					aiMesh->mVertices[i].z
				};
			}
			if (aiMesh->HasNormals()) {
				vertices[i].Normal = {
					aiMesh->mNormals[i].x,
					aiMesh->mNormals[i].y,
					aiMesh->mNormals[i].z
				};
			}
		}


		std::vector<std::uint64_t> indices(aiMesh->mNumFaces);
		// Extract the index buffer.
		if (aiMesh->HasFaces()) {
			for (i = 0; i < aiMesh->mNumFaces; ++i) {
				const aiFace& face = aiMesh->mFaces[i];

				// Only extract triangular faces
				if (face.mNumIndices == 3) {
					indices.push_back(face.mIndices[0]);
					indices.push_back(face.mIndices[1]);
					indices.push_back(face.mIndices[2]);
				}
			}
		}

		//TODO: implement in CEVertex Struct
		/*

		if (aiMesh->HasTangentsAndBitangents()) {
			for (i = 0; i < aiMesh->mNumVertices; ++i) {
				vertexData[i].Tangent = {aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z};
				vertexData[i].Bitangent = {
					aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z
				};
			}
		}

		if (aiMesh->HasTextureCoords(0)) {
			for (i = 0; i < aiMesh->mNumVertices; ++i) {
				vertexData[i].TexCoord = {
					aiMesh->mTextureCoords[0][i].x,
					aiMesh->mTextureCoords[0][i].y,
					aiMesh->mTextureCoords[0][i].z
				};
			}
		}
		 */

		nodes.push_back({vertices, indices});
	}


	return nodes;
}

const aiScene* CEDX12Manager::LoadModelFromFile(const std::string fileName) const {
	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream modelsPathStream;
	modelsPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Models\\" <<
		fileName;
	auto modelPath = modelsPathStream.str();
	spdlog::info("Loading Model: {}", modelPath);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_GenBoundingBoxes);

	// If the import failed, report it
	if (!scene) {
		OutputDebugStringA("ERROR WHILE LOADING MODEL");
		OutputDebugStringA(importer.GetErrorString());
		spdlog::error(importer.GetErrorString());
	}

	return scene;
}

UINT64 CEDX12Manager::GetFenceValue() const {
	return m_currentFence;
}

void CEDX12Manager::SetFenceValue(UINT64 newFence) {
	m_currentFence = newFence;
}

CEDX12Manager::CEDX12Manager(Game::CEWindow& window) : m_window(window),
                                                       m_tearingSupported(false),
                                                       m_rayTracingSupported(false),
                                                       m_adapterIDOverride(UINT_MAX),
                                                       m_adapterID(UINT_MAX),
                                                       m_minFeatureLevel(D3D_FEATURE_LEVEL_11_0),
                                                       m_featureLevel(D3D_FEATURE_LEVEL_11_0) {
	spdlog::info("ConceptEngineFramework DirectX 12 class created.");
}


CEDX12Manager::~CEDX12Manager() {
	CEDX12Manager::Destroy();
}

ID3D12Resource* CEDX12Manager::CurrentBackBuffer() const {
	return m_swapChainBuffer[m_currentBackBuffer].Get();
}

void CEDX12Manager::FlushCommandQueue() {
	//Advance fence value to mark commands up to fence point;
	m_currentFence++;

	//Add instruction to command queue to set new fence point. Because we are on GPU timeline, new fence point will not be set until GPU finishes processign all commands prior to Signal()
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFence));

	//Wait until GPU has completed commands up to this fence point
	if (m_fence->GetCompletedValue() < m_currentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

		//Fire event when GPU hits current fence;
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void CEDX12Manager::ResetCommandList() const {
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

void CEDX12Manager::EnableDebugLayer() const {
#if defined(DEBUG) || defined(_DEBUG)
	wrl::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif
}

/*
 * Objects Creators
 */
void CEDX12Manager::CreateDXGIFactory() {
	bool debugDXGI = false;
#if defined(_DEBUG)
	ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
		debugDXGI = true;
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));

		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
		dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
	}
#endif

	if (!debugDXGI) {
		CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));
	}
}

void CEDX12Manager::CreateDevice() {
	HRESULT hr = D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice));

	if (FAILED(hr)) {
		wrl::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), m_minFeatureLevel, IID_PPV_ARGS(&m_d3dDevice)));
	}

#ifndef NDEBUG
	// Configure debug device (if active).
	ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue))) {
#ifdef _DEBUG
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D12_MESSAGE_ID hide[] = {
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif
}

void CEDX12Manager::CreateAdapter() {
	ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_dxgiFactory.As(&factory6);
	if (FAILED(hr)) {
		throw std::exception("DXGI 1.6 not supported");
	}
	//Create Adapter by GPU preference
	for (UINT adapterID = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
		     adapterID, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter)); ++adapterID) {
		if (m_adapterIDOverride != UINT_MAX && adapterID != m_adapterIDOverride) {
			continue;
		}

		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(m_adapter->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), m_minFeatureLevel, _uuidof(ID3D12Device), nullptr))) {
			m_adapterID = adapterID;
			m_adapterDescription = desc.Description;
#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls", adapterID, desc.VendorId,
			           desc.DeviceId, desc.Description);
			std::string sBuff(std::begin(buff), std::end(buff));
			// spdlog::info(sBuff);
			std::wstringstream wss;
			wss << "Direct3D Adapter (" << adapterID << "): VID:" << desc.VendorId << ", PID:" << desc.DeviceId <<
				" - GPU: "
				<< desc.Description;
			auto ws(wss.str());
			const std::string s(ws.begin(), ws.end());
			spdlog::info(s);
#endif
			break;
		}
	}

	//Create Adapter WARP
#if !defined(NDEBUG)
	if (!m_adapter && m_adapterIDOverride == UINT_MAX) {
		// Try WARP12 instead
		if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&m_adapter)))) {
			spdlog::error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
			throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
		}

		spdlog::info("Direct3D Adapter - WARP 12");
	}
#endif

	if (!m_adapter) {
		if (m_adapterIDOverride != UINT_MAX) {
			throw std::exception("Unavailable adapter requested.");
		}
		else {
			throw std::exception("Unavailable adapter.");
		}
	}
}

void CEDX12Manager::CreateFence() {
	ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}

void CEDX12Manager::CreateDescriptorSizes() {
	m_descriptorSizes = {
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		},
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		},
		{
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		}
	};
}

void CEDX12Manager::CreateCommandQueue() {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
}

void CEDX12Manager::CreateCommandAllocator() {
	ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_commandAllocator.GetAddressOf())
		)
	);

}

void CEDX12Manager::CreateCommandList() {
	ThrowIfFailed(m_d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_commandAllocator.Get(), // Associated command allocator
			nullptr, // Initial PipelineStateObject
			IID_PPV_ARGS(m_commandList.GetAddressOf())
		)
	);

	m_commandList->SetName(L"Main DX12 Command List");

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_commandList->Close();
}

void CEDX12Manager::CreateSwapChain() {
	m_swapChain.Reset();

	// Get the factory that was used to create the adapter.
	wrl::ComPtr<IDXGIFactory> dxgiFactory;
	wrl::ComPtr<IDXGIFactory5> dxgiFactory5;
	ThrowIfFailed(m_adapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
	// Now get the DXGIFactory5 so I can use the IDXGIFactory5::CheckFeatureSupport method.
	ThrowIfFailed(dxgiFactory.As(&dxgiFactory5));

	RECT windowRect;
	::GetClientRect(m_window.GetWindowHandle(), &windowRect);

	m_window.SetWidth(windowRect.right - windowRect.left);
	m_window.SetHeight(windowRect.bottom - windowRect.top);

	auto* hwnd = m_window.GetWindowHandle();

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_window.GetWidth();
	swapChainDesc.Height = m_window.GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {m_numerator, m_denominator};
	fsSwapChainDesc.Windowed = TRUE;
	// Now create the swap chain.
	Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
	ThrowIfFailed(dxgiFactory5->CreateSwapChainForHwnd(m_commandQueue.Get(),
	                                                   hwnd,
	                                                   &swapChainDesc,
	                                                   &fsSwapChainDesc,
	                                                   nullptr,
	                                                   &dxgiSwapChain1));
	ThrowIfFailed(dxgiSwapChain1.As(&m_swapChain));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> CEDX12Manager::GetStaticSamplers() {
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
		0.0f, // mipLODBias
		8); // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
		0.0f, // mipLODBias
		8); // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp
	};
}

void CEDX12Manager::CreateRTVDescriptorHeap(UINT numDescriptors) {
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = numDescriptors;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf())));
}

void CEDX12Manager::CreateDSVDescriptorHeap() {
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf())));
}

void CEDX12Manager::CreateCSVDescriptorHeap(UINT numDescriptors) {
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
}

void CEDX12Manager::CreateSRVDescriptorHeap(UINT numDescriptors) {
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = numDescriptors;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));
}

void CEDX12Manager::CreateConstantBuffers(D3D12_GPU_VIRTUAL_ADDRESS cbAddress,
                                          UINT sizeInBytes,
                                          D3D12_CPU_DESCRIPTOR_HANDLE* handle) const {
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = sizeInBytes;

	if (handle == nullptr) {
		auto defaultHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
		handle = &defaultHandle;
	}
	m_d3dDevice->CreateConstantBufferView(&cbvDesc, *handle);
}

Microsoft::WRL::ComPtr<ID3D12Resource> CEDX12Manager::CreateDefaultBuffer(const void* initData, UINT64 byteSize,
                                                                          Microsoft::WRL::ComPtr<ID3D12Resource>&
                                                                          uploadBuffer) const {

	ComPtr<ID3D12Resource> defaultBuffer;

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

	// Create the actual default buffer resource.
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// In order to copy CPU memory data into our default buffer, we need to create
	// an intermediate upload heap. 
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&heapUpload,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


	// Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	auto copyDestTransition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
	                                                               D3D12_RESOURCE_STATE_COMMON,
	                                                               D3D12_RESOURCE_STATE_COPY_DEST);

	auto genericReadTransition = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
	                                                                  D3D12_RESOURCE_STATE_COPY_DEST,
	                                                                  D3D12_RESOURCE_STATE_GENERIC_READ);

	// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.
	m_commandList->ResourceBarrier(1, &copyDestTransition);
	UpdateSubresources<1>(m_commandList.Get(),
	                      defaultBuffer.Get(),
	                      uploadBuffer.Get(),
	                      0,
	                      0,
	                      1,
	                      &subResourceData);
	m_commandList->ResourceBarrier(1, &genericReadTransition);

	// Note: uploadBuffer has to be kept alive after the above function calls because
	// the command list has not been executed yet that performs the actual copy.
	// The caller can Release the uploadBuffer after it knows the copy has been executed.

	return defaultBuffer;

}

ID3D12RootSignature* CEDX12Manager::CreateRootSignature(D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc) const {
	/*
	 * Shader programs typically require resources as input (constant buffers, textures, samplers).
	 * Root signature defines resources the shader programs expect.
	 * If we think of shader programs as function, and input resources as function parameters, then root signature can be thought of as defining function signature
	 */

	ID3D12RootSignature* rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(rootSignatureDesc,
	                                         D3D_ROOT_SIGNATURE_VERSION_1,
	                                         serializedRootSignature.GetAddressOf(),
	                                         errorBlob.GetAddressOf());
	if (errorBlob != nullptr) {
		spdlog::error((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(m_d3dDevice->CreateRootSignature(0,
	                                               serializedRootSignature->GetBufferPointer(),
	                                               serializedRootSignature->GetBufferSize(),
	                                               IID_PPV_ARGS(&rootSignature)));
	return rootSignature;
}

//TODO: Implement Shader Model 6 usage with DirectXShaderCompiler
//TODO: References: https://asawicki.info/news_1719_two_shader_compilers_of_direct3d_12
//TODO: References: https://github.com/Microsoft/DirectXShaderCompiler
ID3DBlob* CEDX12Manager::CompileShaders(const std::string& fileName,
                                        const D3D_SHADER_MACRO* defines,
                                        const std::string& entryPoint,
                                        const std::string& target) const {

	const auto currentPath = fs::current_path().parent_path().string();
	std::stringstream shadersPathStream;
	shadersPathStream << currentPath << "\\ConceptEngineFramework\\Graphics\\DirectX12\\Resources\\Shaders\\" <<
		fileName;
	auto shaderPath = shadersPathStream.str();
	spdlog::info("Loading Shader: {}", shaderPath);
	const std::wstring wShaderPath(shaderPath.begin(), shaderPath.end());
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;

	hr = D3DCompileFromFile(wShaderPath.c_str(),
	                        defines,
	                        D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                        entryPoint.c_str(),
	                        target.c_str(),
	                        compileFlags,
	                        0,
	                        &byteCode,
	                        &errors);
	if (errors != nullptr) {
		OutputDebugStringA((char*)errors->GetBufferPointer());
		spdlog::error((char*)errors->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	return byteCode;
}

/*
 * Support functions
 */
void CEDX12Manager::TearingSupport() {
	BOOL allowTearing = FALSE;
	ComPtr<IDXGIFactory5> factory5;
	HRESULT hr = m_dxgiFactory.As(&factory5);
	if (SUCCEEDED(hr)) {
		hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
	}
	m_tearingSupported = (allowTearing == TRUE);
}

void CEDX12Manager::FeatureLevelSupport() {
	// Determine maximum supported feature level for this device
	static const D3D_FEATURE_LEVEL s_featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};
	HRESULT hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	m_featureLevel = SUCCEEDED(hr) ? featLevels.MaxSupportedFeatureLevel : m_minFeatureLevel;
}

void CEDX12Manager::DirectXRayTracingSupport() {
	ComPtr<ID3D12Device> testDevice;
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

	m_rayTracingSupported = SUCCEEDED(
			D3D12CreateDevice(
				m_adapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&testDevice)
			)
		)
		&& SUCCEEDED(
			testDevice->CheckFeatureSupport(
				D3D12_FEATURE_D3D12_OPTIONS5,
				&featureSupportData,
				sizeof(featureSupportData)
			)
		)
		&& featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

void CEDX12Manager::MultiSamplingSupport() {
	// Check 4X MSAA quality support for back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)
	));
	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
}

/*
 * Log functions
 */
void CEDX12Manager::LogAdapters() {
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring adapterInfo = L"*** Adapter: ";
		adapterInfo += desc.Description;
		std::string sAdapterInfo(adapterInfo.begin(), adapterInfo.end());
		spdlog::info(sAdapterInfo);

		adapterList.push_back(adapter);
		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i) {
		LogAdapterOutputs(adapterList[i]);
		adapterList[i]->Release();
	}
}

void CEDX12Manager::LogAdapterOutputs(IDXGIAdapter* adapter) {
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring outputInfo = L"*** Output: ";
		outputInfo += desc.DeviceName;
		std::string sOutputInfo(outputInfo.begin(), outputInfo.end());
		spdlog::info(sOutputInfo);

		LogOutputDisplayModes(output, m_backBufferFormat);

		output->Release();

		++i;
	}
}

void CEDX12Manager::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format) {
	UINT count = 0;
	UINT flags = 0;

	//Call with nullptr to get list count;
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& mode : modeList) {
		UINT numerator = mode.RefreshRate.Numerator;
		UINT denominator = mode.RefreshRate.Denominator;
		if (numerator > m_numerator) {
			m_numerator = numerator;
		}
		if (denominator > m_denominator) {
			m_denominator = denominator;
		}

		std::wstring modeInfo = L"*** DisplayMode: Width = " + std::to_wstring(mode.Width) + L" " +
			L"Height = " + std::to_wstring(mode.Height) + L" " +
			L"Refresh = " + std::to_wstring(numerator) + L"/" + std::to_wstring(denominator);
		std::string sModeInfo(modeInfo.begin(), modeInfo.end());
		// spdlog::info(sModeInfo);
	}
}

void CEDX12Manager::LogDirectXInfo() const {

	std::string minFeatureLevelName;
	std::string featureLevelName;

	switch (m_minFeatureLevel) {
	case D3D_FEATURE_LEVEL_12_1:
		minFeatureLevelName = "12.1";
		break;
	case D3D_FEATURE_LEVEL_12_0:
		minFeatureLevelName = "12.0";
		break;
	case D3D_FEATURE_LEVEL_11_1:
		minFeatureLevelName = "11.1";
		break;
	case D3D_FEATURE_LEVEL_11_0:
		minFeatureLevelName = "11.0";
		break;
	default:
		minFeatureLevelName = "NO DATA";
		break;
	}

	switch (m_featureLevel) {
	case D3D_FEATURE_LEVEL_12_1:
		featureLevelName = "12.1";
		break;
	case D3D_FEATURE_LEVEL_12_0:
		featureLevelName = "12.0";
		break;
	case D3D_FEATURE_LEVEL_11_1:
		featureLevelName = "11.1";
		break;
	case D3D_FEATURE_LEVEL_11_0:
		featureLevelName = "11.0";
		break;
	default:
		featureLevelName = "NO DATA";
		break;
	}

	const auto* const rayTracingSupported = m_rayTracingSupported ? "TRUE" : "FALSE";
	const auto* const tearingSupported = m_tearingSupported ? "TRUE" : "FALSE";
	spdlog::info("Min Feature Level Support: {}", minFeatureLevelName);
	spdlog::info("Max Feature Level Support: {}", featureLevelName);
	spdlog::info("Ray Tracing Support: {}", rayTracingSupported);
	spdlog::info("Tearing Support: {}", tearingSupported);
	spdlog::info("4x MSAA (MultiSampling) Quality: {}", m_4xMsaaQuality);
	spdlog::info("RefreshRate: {}/{}", m_numerator, m_denominator);
}
