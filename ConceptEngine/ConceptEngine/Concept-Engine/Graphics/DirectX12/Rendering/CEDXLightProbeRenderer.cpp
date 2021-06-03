#include "CEDXLightProbeRenderer.h"

bool ConceptEngine::Graphics::DirectX12::Rendering::CEDXLightProbeRenderer::Create(
	Main::Rendering::CELightSetup lightSetup, Main::Rendering::CEFrameResources& frameResources) {
	return false;
}

void ConceptEngine::Graphics::DirectX12::Rendering::CEDXLightProbeRenderer::RenderSkyLightProbe(
	Main::RenderLayer::CECommandList& commandList, const Main::Rendering::CELightSetup& lightSetup,
	const Main::Rendering::CEFrameResources& resources) {
}

bool ConceptEngine::Graphics::DirectX12::Rendering::CEDXLightProbeRenderer::CreateSkyLightResources(
	Main::Rendering::CELightSetup& lightSetup) {
	return false;
}
