#include "CEDXSkyBoxRenderPass.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXSkyBoxRenderPass::Create(
	const Main::Rendering::CEFrameResources& resources) {
	return false;
}

void CEDXSkyBoxRenderPass::Render(Main::RenderLayer::CECommandList& commandList,
                                  const Main::Rendering::CEFrameResources& frameResources,
                                  const Render::Scene::CEScene& scene) {
}
