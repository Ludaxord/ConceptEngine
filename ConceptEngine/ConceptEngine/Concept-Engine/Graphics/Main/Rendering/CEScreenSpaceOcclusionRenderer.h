#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEScreenSpaceOcclusionRenderer {
	public:
		CEScreenSpaceOcclusionRenderer() = default;
		virtual ~CEScreenSpaceOcclusionRenderer() = default;

		virtual bool Create(const CEFrameResources& resources) = 0;
		void Release();

		virtual void Render(RenderLayer::CECommandList& commandList, CEFrameResources& frameResources)= 0;

		virtual bool ResizeResources(CEFrameResources& resources)= 0;

	protected:
		Core::Common::CERef<RenderLayer::CEComputePipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEComputeShader> SSAOShader;
		Core::Common::CERef<RenderLayer::CEComputePipelineState> BlurHorizontalPSO;
		Core::Common::CERef<RenderLayer::CEComputeShader> BlurHorizontalShader;
		Core::Common::CERef<RenderLayer::CEComputePipelineState> BlurVerticalPSO;
		Core::Common::CERef<RenderLayer::CEComputeShader> BlurVerticalShader;
		Core::Common::CERef<RenderLayer::CEStructuredBuffer> SSAOSamples;
		Core::Common::CERef<RenderLayer::CEShaderResourceView> SSAOSamplesSRV;
		Core::Common::CERef<RenderLayer::CETexture2D> SSAONoiseTexture;
	};
}
