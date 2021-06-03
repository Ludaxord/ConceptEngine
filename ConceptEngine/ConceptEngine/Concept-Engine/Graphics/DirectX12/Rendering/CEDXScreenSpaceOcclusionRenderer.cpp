#include "CEDXScreenSpaceOcclusionRenderer.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXScreenSpaceOcclusionRenderer::Create(const Main::Rendering::CEFrameResources& resources) {
	return false;
}

void CEDXScreenSpaceOcclusionRenderer::Render(Main::RenderLayer::CECommandList& commandList,
                                              Main::Rendering::CEFrameResources& frameResources) {
}

bool CEDXScreenSpaceOcclusionRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
	return false;
}

bool CEDXScreenSpaceOcclusionRenderer::CreateRenderTarget(Main::Rendering::CEFrameResources& frameResources) {
	return false;
}
