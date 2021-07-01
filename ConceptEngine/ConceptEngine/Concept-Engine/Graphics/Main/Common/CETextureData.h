#pragma once
#include "../RenderLayer/CEPipelineState.h"
#include "../RenderLayer/CECommandList.h"

struct CETextureData {
	CERef<ConceptEngine::Graphics::Main::RenderLayer::CEComputePipelineState> PanoramaPSO;
	CERef<ConceptEngine::Graphics::Main::RenderLayer::CEComputeShader> ComputeShader;
	ConceptEngine::Graphics::Main::RenderLayer::CECommandList CommandList;
};

inline CETextureData MainTextureData;
