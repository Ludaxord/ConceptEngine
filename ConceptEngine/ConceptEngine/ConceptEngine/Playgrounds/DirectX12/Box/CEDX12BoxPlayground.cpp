#include "CEDX12BoxPlayground.h"

#include <DirectXColors.h>


#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEDefaultBuffer.h"
#include "../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

using namespace ConceptEngineFramework::Graphics::DirectX12;

const int gNumFrameResources = 3;

CEDX12BoxPlayground::CEDX12BoxPlayground() : CEDX12Playground() {
}

void CEDX12BoxPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_dx12manager->CreateCSVDescriptorHeap();

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	mObjectCB = std::make_unique<Resources::CEUploadBuffer<Resources::CEObjectConstants>>(d3dDevice.Get(), 1, true);

	UINT objCBByteSize = (sizeof(Resources::CEObjectConstants) + 255) & ~255;

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	m_dx12manager->CreateConstantBuffers(cbAddress, objCBByteSize);

	//Root parameter can be a table, root descriptor or root constants
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter,
	                                        0,
	                                        nullptr,
	                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();
}

void CEDX12BoxPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
}

void CEDX12BoxPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12BoxPlayground::BuildShadersAndInputLayout() {
	HRESULT hr = S_OK;
	
	const auto currentPath = fs::current_path();
	std::stringstream ss;
	ss << "CURRENT PATH: " << currentPath;
	spdlog::info(ss.str());


	m_vsByteCode = m_dx12manager->CompileShaders(L"Graphics\\DirectX12\\Resources\\Shaders\\CEBoxVertexShader.hlsl", nullptr, "VS", "vs_6_3");
	m_psByteCode = m_dx12manager->CompileShaders(L"Graphics\\DirectX12\\Resources\\Shaders\\CEBoxPixelShader.hlsl", nullptr, "PS", "ps_6_3");

	m_inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void CEDX12BoxPlayground::BuildBoxGeometry() {

	auto d3dDevice = m_dx12manager->GetD3D12Device();
	auto d3dCommandList = m_dx12manager->GetD3D12CommandList();

	std::vector<Resources::CEVertex> vertices = {
		Resources::CEVertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::White)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Red)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(DirectX::Colors::DarkRed)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(DirectX::Colors::Green)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Blue)}),
		Resources::CEVertex({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Yellow)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(DirectX::Colors::Magenta)}),
		Resources::CEVertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(DirectX::Colors::AliceBlue)}),
	};

	std::vector<std::uint16_t> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_boxGeo = std::make_unique<Resources::MeshGeometry>();
	m_boxGeo->Name = "GeoBox";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_boxGeo->VertexBufferCPU));
	CopyMemory(m_boxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_boxGeo->IndexBufferCPU));
	CopyMemory(m_boxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	const auto defaultVertexBuffer = new Resources::CEDefaultBuffer(d3dDevice.Get(),
	                                                                d3dCommandList.Get(),
	                                                                vertices.data(),
	                                                                vbByteSize,
	                                                                m_boxGeo->VertexBufferUploader);
	m_boxGeo->VertexBufferGPU = defaultVertexBuffer->Resource();

	const auto defaultIndexBuffer = new Resources::CEDefaultBuffer(d3dDevice.Get(),
	                                                               d3dCommandList.Get(),
	                                                               indices.data(),
	                                                               ibByteSize,
	                                                               m_boxGeo->IndexBufferUploader);
	m_boxGeo->IndexBufferGPU = defaultIndexBuffer->Resource();

	m_boxGeo->VertexByteStride = sizeof(Resources::CEVertex);
	m_boxGeo->VertexBufferByteSize = vbByteSize;
	m_boxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_boxGeo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	m_boxGeo->DrawArgs["box"] = subMesh;
}

//TODO: Add RayTraced StateObject
void CEDX12BoxPlayground::BuildPSO() {
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = {
		static_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize()
	};
	psoDesc.PS = {
		static_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
	psoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
	psoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}
