#include "CEDXRayTracing.h"

//TODO: Implement!!!
ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::CEDXRayTracingGeometry(CEDXDevice* device,
	uint32 flags): CERayTracingGeometry(flags), CEDXDeviceElement(device), VertexBuffer(nullptr), IndexBuffer(nullptr),
	               ResultBuffer(nullptr), ScratchBuffer(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::Build(CEDXCommandContext& commandContext,
	bool update) {
	Assert(VertexBuffer != nullptr);

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc;
	Memory::CEMemory::Memzero(&geometryDesc);

	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.VertexBuffer.StartAddress = VertexBuffer->GetResource()->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = VertexBuffer->GetStride();
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = VertexBuffer->GetNumVertices();
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	if (IndexBuffer) {
		CEIndexFormat IndexFormat = IndexBuffer->GetFormat();
		geometryDesc.Triangles.IndexFormat = IndexFormat == CEIndexFormat::uint32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		geometryDesc.Triangles.IndexBuffer = IndexBuffer->GetResource()->GetGPUVirtualAddress();
		geometryDesc.Triangles.IndexCount = IndexBuffer->GetNumIndices();
	}

	//TODO: Implement

	return false;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::CEDXShaderBindingTableBuilder(
	CEDXDevice* device): CEDXDeviceElement(device) {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::PopulateEntry(
	CEDXRayTracingPipelineState* pipelineState, CEDXRootSignature* rootSignature,
	CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
	CEDXShaderBindingTableEntry& shaderBindingEntry, const CERayTracingShaderResources& resources) {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::CopyDescriptors() {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXShaderBindingTableBuilder::Reset() {
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::CEDXRayTracingScene(CEDXDevice* device,
	uint32 flags): CERayTracingScene(flags), CEDXDeviceElement(device), ShaderBindingTableBuilder(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::Build(CEDXCommandContext& commandContext,
	const CERayTracingGeometryInstance* instances, uint32 numInstances, bool update) {
	return false;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::BuildBindingTable(
	CEDXCommandContext& commandContext, CEDXRayTracingPipelineState* pipelineState,
	CEDXOnlineDescriptorHeap* resourceHeap, CEDXOnlineDescriptorHeap* samplerHeap,
	const CERayTracingShaderResources* rayGenLocalResources, const CERayTracingShaderResources* missLocalResources,
	const CERayTracingShaderResources* hitGroupResources, uint32 numHitGroupResources) {
	return false;
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::SetName(const std::string& name) {
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetRayGenShaderRecord() const {
	return {};
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetMissShaderTable() const {
	return {};
}

D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingScene::
GetHitGroupTable() const {
	return {};
}
