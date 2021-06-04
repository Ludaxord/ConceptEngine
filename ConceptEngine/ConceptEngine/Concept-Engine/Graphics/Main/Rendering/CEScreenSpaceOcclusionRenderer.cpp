#include "CEScreenSpaceOcclusionRenderer.h"

void ConceptEngine::Graphics::Main::Rendering::CEScreenSpaceOcclusionRenderer::Release() {
	PipelineState.Reset();
	BlurHorizontalPSO.Reset();
	BlurVerticalPSO.Reset();
	SSAOSamples.Reset();
	SSAOSamplesSRV.Reset();
	SSAONoiseTexture.Reset();
	SSAOShader.Reset();
	BlurHorizontalShader.Reset();
	BlurVerticalShader.Reset();
}
