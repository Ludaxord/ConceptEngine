#include "CEDeferredRenderer.h"

using namespace ConceptEngine::Graphics::Main::Rendering;

void CEDeferredRenderer::Release() {
	PipelineState.Reset();
	PrePassPipelineState.Reset();
	TiledLightPassPSO.Reset();
	BaseVertexShader.Reset();
	BasePixelShader.Reset();
	PrePassVertexShader.Reset();
	TiledLightPassPSODebug.Reset();
	TiledLightShader.Reset();
	TiledLightDebugShader.Reset();
}
