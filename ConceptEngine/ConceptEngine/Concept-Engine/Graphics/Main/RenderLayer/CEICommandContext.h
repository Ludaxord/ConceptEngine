#pragma once

#include "CERendering.h"
#include "CEResourceViews.h"
#include "CETexture.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Math/CEColor.h"
#include "../RenderLayer/CERayTracing.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEICommandContext : public Core::Common::CERefCountedObject {
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) = 0;
		virtual void EndTimeStamp(CEGPUProfiler* profiler, uint32 index) = 0;

		virtual void ClearRenderTargetView(CERenderTargetView* renderTargetView, Math::CEColorF& clearColor) = 0;
		virtual void ClearDepthStencilView(CEDepthStencilView* depthStencilView, const CEDepthStencilF& clearColor) = 0;
		virtual void ClearUnorderedAccessViewFloat(CEUnorderedAccessView* unorderedAccessView,
		                                           const Math::CEColorF& clearColor) = 0;

		virtual void SetShadingRate(CEShadingRate shadingRate) = 0;
		virtual void SetShadingRateImage(CETexture2D* shadingImage) = 0;

		virtual void SetViewport(float width, float height, float minDepth, float maxDepth, float x, float y) = 0;
		virtual void SetScissorRect(float width, float height, float x, float y) = 0;

		virtual void SetBlendFactor(const Math::CEColorF& color) = 0;

		virtual void BeginRenderPass() = 0;
		virtual void EndRenderPass() = 0;

		virtual void SetPrimitiveTopology(CEPrimitiveTopology primitiveTopologyType) = 0;

		virtual void SetVertexBuffers(CEVertexBuffer* const* vertexBuffers, uint32 bufferCount, uint32 bufferSlot) = 0;
		virtual void SetIndexBuffer(CEIndexBuffer* indexBuffer) = 0;

		virtual void SetRenderTargets(CERenderTargetView* const * renderTargetView, uint32 renderTargetCount,
		                              CEDepthStencilView* depthStencilView) = 0;

		virtual void SetRayTracingBindings(
			CERayTracingScene* rayTracingScene,
			CERayTracingPipelineState* pipelineState,
			const CERayTracingShaderResources* globalResource,
			const CERayTracingShaderResources* rayGenLocalResources,
			const CERayTracingShaderResources* missLocalResources,
			const CERayTracingShaderResources* hitGroupResources,
			uint32 numHitGroupResources
		) = 0;


	};
}
