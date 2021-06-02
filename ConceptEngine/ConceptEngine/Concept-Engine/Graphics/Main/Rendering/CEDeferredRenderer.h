#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEDeferredRenderer {
	public:
		CEDeferredRenderer() = default;
		~CEDeferredRenderer() = default;


		bool Create(CEFrameResources& FrameResources);
		void Release();

		void RenderPrePass(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources);
		void RenderBasePass(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources);
		void RenderDeferredTiledLightPass(RenderLayer::CECommandList& commandList,
		                                  const CEFrameResources& frameResources, const CELightSetup& lightSetup);

		bool ResizeResources(CEFrameResources& resources);

	private:
		bool CreateGBuffer(CEFrameResources& frameResources);

		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> BaseVertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> BasePixelShader;
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PrePassPipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> PrePassVertexShader;
		Core::Common::CERef<RenderLayer::CEComputePipelineState> TiledLightPassPSO;
		Core::Common::CERef<RenderLayer::CEComputeShader> TiledLightShader;
		Core::Common::CERef<RenderLayer::CEComputePipelineState> TiledLightPassPSODebug;
		Core::Common::CERef<RenderLayer::CEComputeShader> TiledLightDebugShader;
	};
}
