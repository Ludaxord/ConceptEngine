#include "CEDX12LitShapesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12LitShapesPlayground::CEDX12LitShapesPlayground() : CEDX12Playground() {
}

void CEDX12LitShapesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list
	m_dx12manager->ResetCommandList();

	//Build root signature
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	//Root parameter can be a table, root descriptor or root constants
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);

	//Root signature is an array of root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(3,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	//Build shaders and inputs layout
	BuildShadersAndInputLayout("CELitVertexShader.hlsl", "CELitPixelShader.hlsl");

	//build shapes geometry
	BuildShapeGeometry();
	BuildModelGeometry();

	//build materials
	BuildMaterials();

	//build render items
	BuildRenderItems();

	//build frame resources
	BuildFrameResources();

	//build pso
	BuildPSOs(m_rootSignature);

	//execute command list
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12LitShapesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
}

void CEDX12LitShapesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12LitShapesPlayground::Resize() {
	CEDX12Playground::Resize();
}

void CEDX12LitShapesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
}

void CEDX12LitShapesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
}

void CEDX12LitShapesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
}

void CEDX12LitShapesPlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12LitShapesPlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12LitShapesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12LitShapesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
}

void CEDX12LitShapesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12LitShapesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12LitShapesPlayground::BuildShapeGeometry() {
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	// Define the SubmeshGeometry that cover different 
	// regions of the vertex/index buffers.

	Resources::SubMeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	Resources::SubMeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	Resources::SubMeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	Resources::SubMeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Resources::CEVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "shapeGeo";

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

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12LitShapesPlayground::BuildModelGeometry() {
	auto node = m_dx12manager->LoadNode("BrickLandspeeder4501.obj");
	spdlog::info("NODES: VERTEX {} INDEX {}", node.vertices.size(), node.indices.size());

	const UINT vbByteSize = (UINT)node.vertices.size() * sizeof(Resources::CENormalVertex);
	const UINT ibByteSize = (UINT)node.indices.size() * sizeof(std::uint64_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "legoGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), node.vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), node.indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(node.vertices.data(), vbByteSize,
	                                                          geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(node.indices.data(), ibByteSize,
	                                                         geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_FLOAT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)node.indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["lego"] = subMesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12LitShapesPlayground::BuildMaterials() {
}

void CEDX12LitShapesPlayground::BuildRenderItems() {
}

void CEDX12LitShapesPlayground::BuildFrameResources() {
}
