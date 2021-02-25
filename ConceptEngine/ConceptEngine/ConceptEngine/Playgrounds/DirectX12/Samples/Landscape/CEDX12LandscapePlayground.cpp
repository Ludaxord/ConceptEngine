#include "CEDX12LandscapePlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"
#include "../../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

void CEDX12LandscapePlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create basic waves

	//Create Root Signature

	//Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	//Create root ConstantBufferView
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	//root signature is an array of root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//Create root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	//Create Shaders and input layout
	BuildShadersAndInputLayout();

	//Create Landscape geometry
	BuildLandscapeGeometry();

	//Create geometry buffers

	//Create Render items

	//Create Frame resources

	//Create Pipeline State Objects
}

void CEDX12LandscapePlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);

}

void CEDX12LandscapePlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LandscapePlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LandscapePlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12LandscapePlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12LandscapePlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LandscapePlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12LandscapePlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

//TODO: Implement Shader Model 6 usage with DirectXShaderCompiler
//TODO: References: https://asawicki.info/news_1719_two_shader_compilers_of_direct3d_12
//TODO: References: https://github.com/Microsoft/DirectXShaderCompiler
void CEDX12LandscapePlayground::BuildShadersAndInputLayout() {
	m_shadersMap["VS"] = m_dx12manager->CompileShaders("CELandscapeVertexShader.hlsl",
	                                                   nullptr,
	                                                   "VS",
	                                                   // "vs_6_3"
	                                                   "vs_5_0"
	);
	m_shadersMap["PS"] = m_dx12manager->CompileShaders("CELandscapePixelShader.hlsl",
	                                                   nullptr,
	                                                   "PS",
	                                                   // "ps_6_3"
	                                                   "ps_5_0"
	);

	m_inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void CEDX12LandscapePlayground::BuildLandscapeGeometry() {
	GeometryGenerator geometry;
	GeometryGenerator::MeshData grid = geometry.CreateGrid(160.0f, 160.0f, 50, 50);

	/*
	 * Extract vertex elements we are interested and apply height function to each vertex
	 * In addition, color vertices based on their height so we have sandy looking beaches, grassy low hills, and snow mountain peaks
	 */

	std::vector<Resources::CEVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i) {
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

		//Color vertex based on its height
		if (vertices[i].Pos.y < -10.0f) {
			//Sandy beach color
			vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (vertices[i].Pos.y < 5.0f) {
			//Light yellow green
			vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (vertices[i].Pos.y < 12.0f) {
			//Dark yellow-green
			vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (vertices[i].Pos.y < 20.0f) {
			//Dark brown
			vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else {
			//White snow
			vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "landscapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CEVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = subMesh;

	m_geometries["landscapeGeo"] = std::move(geo);
}

float CEDX12LandscapePlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12LandscapePlayground::GetHillsNormal(float x, float z) const {
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z)
	);

	XMVECTOR uintNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, uintNormal);

	return n;
}
