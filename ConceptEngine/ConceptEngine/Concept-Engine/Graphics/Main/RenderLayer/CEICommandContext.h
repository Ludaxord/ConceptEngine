#pragma once

#include "CERendering.h"
#include "CEResourceViews.h"
#include "CESamplerState.h"
#include "CEShader.h"
#include "CETexture.h"
#include "../../../Core/Common/CERefCountedObject.h"
#include "../../../Math/CEColor.h"
#include "../RenderLayer/CERayTracing.h"

#include "../RenderLayer/CEBuffer.h"

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

		virtual void SetGraphicsPipelineState(class CEGraphicsPipelineState* pipelineState) = 0;
		virtual void SetComputePipelineState(class CEComputePipelineState* pipelineState) = 0;

		virtual void Set32BitShaderConstants(CEShader* shader, const void* shader32BitConstants,
		                                     uint32 num32BitConstants) = 0;

		virtual void SetShaderResourceView(CEShader* shader, CEShaderResourceView* ce_shader_resource_view,
		                                   uint32 parameterIndex) = 0;
		virtual void SetShaderResourceViews(CEShader* ce_shader, CEShaderResourceView* const* ce_shader_resource_view,
		                                    uint32 numShaderResourceViews, uint32 parameterIndex) = 0;

		virtual void SetUnorderedAccessView(CEShader* shader, CEUnorderedAccessView* unorderedAccessView,
		                                    uint32 parameterIndex) = 0;
		virtual void SetUnorderedAccessViews(CEShader* shader, CEUnorderedAccessView* const* unorderedAccessViews,
		                                     uint32 numUnorderedAccessViews, uint32 parameterIndex) = 0;

		virtual void SetConstantBuffer(CEShader* shader, CEConstantBuffer* constantBuffer, uint32 parameterIndex) = 0;
		virtual void SetConstantBuffers(CEShader* shader, CEConstantBuffer* const* constantBuffers,
		                                uint32 numConstantBuffers, uint32 parameterIndex) = 0;

		virtual void SetSamplerState(CEShader* shader, CESamplerState* samplerState, uint32 parameterIndex) = 0;
		virtual void SetSamplerStates(CEShader* shader, CESamplerState* const* samplerStates, uint32 numSamplerStates,
		                              uint32 parameterIndex) = 0;

		virtual void ResolveTexture(CETexture* destination, CETexture* source) = 0;

		virtual void UpdateBuffer(CEBuffer* destination, uint64 offsetInBytes, uint64 sizeInBytes,
		                          const void* sourceData) = 0;
		virtual void UpdateTexture2D(CETexture2D* destination, uint32 width, uint32 height, uint32 mipLevel,
		                             const void* sourceData) = 0;

		virtual void CopyBuffer(CEBuffer* destination, CEBuffer* source, const CECopyBufferInfo& copyInfo) = 0;
		virtual void CopyTexture(CETexture* destination, CETexture* source) = 0;
		virtual void CopyTextureRegion(CETexture* destination, CETexture* source,
		                               const CECopyTextureInfo& copyTexture) = 0;

		virtual void DiscardResource(class CEResource* resource) = 0;

		virtual void BuildRayTracingGeometry(CERayTracingGeometry* geometry, CEVertexBuffer* vertexBuffer,
		                                     CEIndexBuffer* indexBuffer, bool update) = 0;
		virtual void BuildRayTracingScene(CERayTracingScene* scene, const CERayTracingGeometryInstance* instances,
		                                  uint32 numInstances, bool update) = 0;

		virtual void GenerateMips(CETexture* texture) = 0;

		virtual void TransitionTexture(CETexture* texture, CEResourceState beforeState, CEResourceState afterState) = 0;
		virtual void TransitionBuffer(CEBuffer* buffer, CEResourceState beforeState, CEResourceState afterState) = 0;

		virtual void UnorderedAccessTextureBarrier(CETexture* texture) = 0;
		virtual void UnorderedAccessBufferBarrier(CEBuffer* buffer) = 0;

		virtual void Draw(uint32 vertexCount, uint32 startVertexLocation) = 0;
		virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, int32 baseVertexLocation) = 0;
		virtual void DrawInstanced(uint32 vertexPerCountInstance, uint32 instanceCount, uint32 startVertexLocation,
		                           uint32 startInstanceLocation) = 0;
		virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,
		                                  uint32 baseVertexLocation, uint32 startInstanceLocation) = 0;

		virtual void Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) = 0;

		virtual void DispatchRays(CERayTracingScene* rayTracingScene, CERayTracingPipelineState* pipelineState,
		                          uint32 width, uint32 height, uint32 depth) = 0;

		virtual void SetRayTracingBindings(
			CERayTracingScene* rayTracingScene,
			CERayTracingPipelineState* pipelineState,
			const CERayTracingShaderResources* globalResource,
			const CERayTracingShaderResources* rayGenLocalResources,
			const CERayTracingShaderResources* missLocalResources,
			const CERayTracingShaderResources* hitGroupResources,
			uint32 numHitGroupResources
		) = 0;

		virtual void ClearState() = 0;
		virtual void Flush() = 0;

		virtual void InsertMaker(const std::string& message) = 0;

		virtual void BeginExternalCapture() = 0;
		virtual void EndExternalCapture() = 0;

	};
}
