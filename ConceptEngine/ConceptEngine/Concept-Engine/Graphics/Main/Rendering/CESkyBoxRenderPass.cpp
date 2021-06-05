#include "CESkyBoxRenderPass.h"

void ConceptEngine::Graphics::Main::Rendering::CESkyBoxRenderPass::Release() {
	PipelineState.Reset();
	SkyboxIndexBuffer.Reset();
	SkyboxVertexBuffer.Reset();
	SkyboxSampler.Reset();
	SkyboxPixelShader.Reset();
	SkyboxVertexShader.Reset();
}
