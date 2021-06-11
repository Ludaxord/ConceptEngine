#include "CEDXRayTracing.h"

//TODO: Implement!!!
ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::CEDXRayTracingGeometry(CEDXDevice* device,
	uint32 flags): CERayTracingGeometry(flags), CEDXDeviceElement(device) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingGeometry::Build(CEDXCommandContext& commandContext,
	bool update) {
	return false;
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
