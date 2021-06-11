#include "CEDXPipelineState.h"

//TODO: Implement!!
ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGraphicsPipelineState::CEDXGraphicsPipelineState(
	CEDXDevice* device): CEDXDeviceElement(device), PipelineState(nullptr), RootSignature(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGraphicsPipelineState::Create(
	const CEGraphicsPipelineStateCreateInfo& createInfo) {
	return false;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::CEDXComputePipelineState(CEDXDevice* device,
	const CERef<CEDXComputeShader>& shader): CEComputePipelineState(), CEDXDeviceElement(device),
	                                         PipelineState(nullptr), Shader(shader), RootSignature(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::Create() {
	return false;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::CEDXRayTracingPipelineState(
	CEDXDevice* device): CEDXDeviceElement(device), StateObject(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::Create(
	const CERayTracingPipelineStateCreateInfo& createInfo) {
	return false;
}

void* ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::GetShaderIdentifier(
	const std::string& exportName) {
	return nullptr;
}
