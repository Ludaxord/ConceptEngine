#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEDeferredRenderer {
	public:
		CEDeferredRenderer() = default;
		virtual ~CEDeferredRenderer() = default;


		virtual bool Create(CEFrameResources& FrameResources) = 0;
		void Release();

		virtual void RenderPrePass(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources) = 0;
		virtual void RenderBasePass(RenderLayer::CECommandList& commandList,
		                            const CEFrameResources& frameResources) = 0;
		virtual void RenderDeferredTiledLightPass(RenderLayer::CECommandList& commandList,
		                                          const CEFrameResources& frameResources,
		                                          const CELightSetup& lightSetup) = 0;

		virtual bool ResizeResources(CEFrameResources& resources) = 0;

	protected:
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
