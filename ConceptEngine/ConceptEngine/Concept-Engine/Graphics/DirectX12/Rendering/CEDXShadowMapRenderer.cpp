#include "CEDXShadowMapRenderer.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;



bool CEDXShadowMapRenderer::Create(const Main::Rendering::CELightSetup& lightSetup,
                                   const Main::Rendering::CEFrameResources& resources) {
	return false;
}

void CEDXShadowMapRenderer::RenderPointLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                    const Main::Rendering::CELightSetup& lightSetup,
                                                    const Render::Scene::CEScene& scene) {
}

void CEDXShadowMapRenderer::RenderDirectionalLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                          const Main::Rendering::CELightSetup& lightSetup,
                                                          const Render::Scene::CEScene& scene) {
}

bool CEDXShadowMapRenderer::CreateShadowMaps(Main::Rendering::CELightSetup& frameResources) {
	return false;
}
