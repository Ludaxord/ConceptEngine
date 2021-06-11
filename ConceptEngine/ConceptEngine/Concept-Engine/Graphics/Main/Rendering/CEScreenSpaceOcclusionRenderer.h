#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEScreenSpaceOcclusionRenderer {
	public:
		CEScreenSpaceOcclusionRenderer() = default;
		virtual ~CEScreenSpaceOcclusionRenderer() = default;

		virtual bool Create( CEFrameResources& resources) = 0;
		void Release();

		virtual void Render(RenderLayer::CECommandList& commandList, CEFrameResources& frameResources)= 0;

		virtual bool ResizeResources(CEFrameResources& resources)= 0;

	protected:
		CERef<RenderLayer::CEComputePipelineState> PipelineState;
		CERef<RenderLayer::CEComputeShader> SSAOShader;
		CERef<RenderLayer::CEComputePipelineState> BlurHorizontalPSO;
		CERef<RenderLayer::CEComputeShader> BlurHorizontalShader;
		CERef<RenderLayer::CEComputePipelineState> BlurVerticalPSO;
		CERef<RenderLayer::CEComputeShader> BlurVerticalShader;
		CERef<RenderLayer::CEStructuredBuffer> SSAOSamples;
		CERef<RenderLayer::CEShaderResourceView> SSAOSamplesSRV;
		CERef<RenderLayer::CETexture2D> SSAONoiseTexture;
	};
}
