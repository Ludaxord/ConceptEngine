#pragma once
#include "CEBaseLightSetup.h"
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
		
		virtual void RenderDeferredTiledLightPass(RenderLayer::CECommandList& commandList,
		                                          const CEFrameResources& frameResources,
		                                          const CEBaseLightSetup& lightSetup) = 0;

		virtual bool ResizeResources(CEFrameResources& resources) = 0;

	protected:
		CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		CERef<RenderLayer::CEVertexShader> BaseVertexShader;
		CERef<RenderLayer::CEPixelShader> BasePixelShader;
		CERef<RenderLayer::CEGraphicsPipelineState> PrePassPipelineState;
		CERef<RenderLayer::CEVertexShader> PrePassVertexShader;
		CERef<RenderLayer::CEComputePipelineState> TiledLightPassPSO;
		CERef<RenderLayer::CEComputeShader> TiledLightShader;
		CERef<RenderLayer::CEComputePipelineState> TiledLightPassPSODebug;
		CERef<RenderLayer::CEComputeShader> TiledLightDebugShader;
	};
}
