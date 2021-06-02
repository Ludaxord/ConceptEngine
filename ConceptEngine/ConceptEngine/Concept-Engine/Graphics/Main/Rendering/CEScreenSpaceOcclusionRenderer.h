#pragma once

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEScreenSpaceOcclusionRenderer {
	public:
		CEScreenSpaceOcclusionRenderer() = default;
		~CEScreenSpaceOcclusionRenderer() = default;

		bool Create(const CEFrameResources& resources);
		void Release();

		void Render(RenderLayer::CECommandList& commandList, CEFrameResources& frameResources);

		bool ResizeResources(CEFrameResources& resources);

	private:
		bool CreateRenderTarget(CEFrameResources& frameResources);

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
