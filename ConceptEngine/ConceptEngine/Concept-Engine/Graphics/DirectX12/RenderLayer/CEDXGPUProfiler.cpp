#include "CEDXGPUProfiler.h"

//TODO: Implement
ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::CEDXGPUProfiler(CEDXDevice* device):
	CEDXDeviceElement(device), CEGPUProfiler() {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::GetTimeQuery(TimeQuery& query,
	uint32 index) const {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::BeginQuery(
	ID3D12GraphicsCommandList* commandList, uint32 index) {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::EndQuery(ID3D12GraphicsCommandList* commandList,
	uint32 index) {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::ResolveQueries(
	CEDXCommandContext& commandContext) {
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler* ConceptEngine::Graphics::DirectX12::RenderLayer::
CEDXGPUProfiler::Create(CEDXDevice* device) {
	return nullptr;
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::AllocateReadResources() {
	return false;
}
