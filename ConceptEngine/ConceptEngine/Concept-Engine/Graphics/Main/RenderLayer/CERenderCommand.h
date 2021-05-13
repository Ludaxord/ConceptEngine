#pragma once
#include "CEICommandContext.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	struct CERenderCommand {
		virtual ~CERenderCommand() = default;
		virtual void Execute(CEICommandContext&);

		void operator()(CEICommandContext& commandContext) {
			Execute(commandContext);
		}

		CERenderCommand* NextCommand = nullptr;
	};

	struct CEBeginRenderCommand : public CERenderCommand {
		CEBeginRenderCommand() {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.Begin();
		}

	};

	struct CEEndRenderCommand : public CERenderCommand {
		CEEndRenderCommand() {
			
		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.End();
		}
	};

	struct CEBeginTimeStampRenderCommand : public CERenderCommand {

	};

	struct CEEndTimeStampRenderCommand : public CERenderCommand {

	};

	struct CEClearRenderTargetViewRenderCommand : public CERenderCommand {

	};

	struct CEClearDepthStencilViewRenderCommand : public CERenderCommand {

	};

	struct CEClearUnorderedAccessViewFloatRenderCommand : public CERenderCommand {

	};

	struct CESetShadingRateRenderCommand : public CERenderCommand {

	};

	struct CESetShadingRateImageRenderCommand : public CERenderCommand {

	};

	struct CESetViewportRenderCommand : public CERenderCommand {

	};

	struct CESetScissorRectRenderCommand : public CERenderCommand {

	};

	struct CESetBlendFactorRenderCommand : public CERenderCommand {

	};

	struct CEBeginRenderPassRenderCommand : public CERenderCommand {

	};

	struct CEEndRenderPassRenderCommand : public CERenderCommand {

	};

	struct CESetPrimitiveTopologyRenderCommand : public CERenderCommand {

	};

	struct CESetVertexBuffersRenderCommand : public CERenderCommand {

	};

	struct CESetIndexBufferRenderCommand : public CERenderCommand {

	};

	struct CESetRenderTargetsRenderCommand : public CERenderCommand {

	};

	struct CESetRayTracingBindingsRenderCommand : public CERenderCommand {

	};

	struct CESetGraphicsPipelineStateRenderCommand : public CERenderCommand {

	};

	struct CESetComputePipelineStateRenderCommand : public CERenderCommand {

	};

	struct CESet32BitShaderConstantsRenderCommand : public CERenderCommand {

	};

	struct CESetShaderResourceViewRenderCommand : public CERenderCommand {

	};

	struct CESetShaderResourceViewsRenderCommand : public CERenderCommand {

	};

	struct CESetUnorderedAccessViewRenderCommand : public CERenderCommand {

	};

	struct CESetUnorderedAccessViewsRenderCommand : public CERenderCommand {

	};

	struct CESetConstantBufferRenderCommand : public CERenderCommand {

	};

	struct CESetConstantBuffersRenderCommand : public CERenderCommand {

	};

	struct CESetSamplerStateRenderCommand : public CERenderCommand {

	};

	struct CESetSamplerStatesRenderCommand : public CERenderCommand {

	};

	struct CEResolveTextureRenderCommand : public CERenderCommand {

	};

	struct CEUpdateBufferRenderCommand : public CERenderCommand {

	};

	struct CEUpdateTexture2DRenderCommand : public CERenderCommand {

	};

	struct CECopyBufferRenderCommand : public CERenderCommand {

	};

	struct CECopyTextureRenderCommand : public CERenderCommand {

	};

	struct CEDiscardResourceRenderCommand : public CERenderCommand {

	};

	struct CEBuildRayTracingGeometryRenderCommand : public CERenderCommand {

	};

	struct CEBuildRayTracingSceneRenderCommand : public CERenderCommand {

	};

	struct CEGenerateMipsRenderCommand : public CERenderCommand {

	};

	struct CETransitionTextureRenderCommand : public CERenderCommand {

	};

	struct CETransitionBufferRenderCommand : public CERenderCommand {

	};

	struct CEUnorderedAccessTextureBarrierRenderCommand : public CERenderCommand {

	};

	struct CEUnorderedAccessBufferBarrierRenderCommand : public CERenderCommand {

	};

	struct CEDrawRenderCommand : public CERenderCommand {

	};

	struct CEDrawIndexedRenderCommand : public CERenderCommand {

	};

	struct CEDrawInstancedRenderCommand : public CERenderCommand {

	};

	struct CEDrawIndexedInstancedRenderCommand : public CERenderCommand {

	};

	struct CEDispatchComputeRenderCommand : public CERenderCommand {

	};

	struct CEDispatchRayRenderCommand : public CERenderCommand {

	};

	struct CEInsertCommandListMakerRenderCommand : public CERenderCommand {

	};

	struct CEDebugBreakRenderCommand : public CERenderCommand {

	};

	struct CEBeginExternalCaptureRenderCommand : public CERenderCommand {

	};

	struct CEEndExternalCaptureRenderCommand : public CERenderCommand {

	};
}
