#pragma once
#include "CEBuffer.h"
#include "CEGPUProfiler.h"
#include "CEPipelineState.h"
#include "CERayTracing.h"
#include "CERenderCommand.h"
#include "CERendering.h"
#include "CEResourceViews.h"
#include "CEShader.h"
#include "CETexture.h"
#include "../../../Memory/CELinearAllocator.h"
#include "../../../Memory/CEMemory.h"
#include "../../../Core/Common/CETypes.h"
#include "../../../Core/Containers/CEContainerUtilities.h"
#include "../../../Math/CEColor.h"

#include <boost/function/function0.hpp>

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CECommandList {

		friend class CECommandListExecutor;

	public:
		CECommandList() : CommandAllocator(), FirstCommand(nullptr), LastCommand(nullptr) {

		}

		~CECommandList() {
			Reset();
		}

		void Execute(boost::function<void()> ExecuteFunction) {
			Begin();
			ExecuteFunction();
			End();
		}

		void Begin() {
			Assert(IsRecording == false);
			InsertCommand<CEBeginRenderCommand>();
			IsRecording = true;
		}

		void End() {
			Assert(IsRecording == true);
			InsertCommand<CEEndRenderCommand>();
			IsRecording = false;
		}

		void BeginTimeStamp(CEGPUProfiler* profiler, uint32 index) {
			if (profiler) {
				profiler->AddRef();
			}
			InsertCommand<CEEndTimeStampRenderCommand>(profiler, index);
		}

		void EndTimeStamp(CEGPUProfiler* profiler, uint32 index) {
			if (profiler) {
				profiler->AddRef();
			}
			InsertCommand<CEEndTimeStampRenderCommand>(profiler, index);
		}

		void ClearRenderTargetView(CERenderTargetView* renderTargetView, const Math::CEColorF& clearColor) {
			Assert(renderTargetView != nullptr);
			renderTargetView->AddRef();
			InsertCommand<CEClearRenderTargetViewRenderCommand>(renderTargetView, clearColor);
		}

		void ClearDepthStencilView(CEDepthStencilView* depthStencilView, const CEDepthStencilF& clearValue) {
			Assert(depthStencilView != nullptr);
			depthStencilView->AddRef();
			InsertCommand<CEClearDepthStencilViewRenderCommand>(depthStencilView, clearValue);
		}

		void ClearUnorderedAccessView(CEUnorderedAccessView* unorderedAccessView, const Math::CEColorF& clearColor) {
			Assert(unorderedAccessView != nullptr);
			unorderedAccessView->AddRef();
			InsertCommand<CEClearUnorderedAccessViewFloatRenderCommand>(unorderedAccessView, clearColor);
		}

		void SetShadingRate(CEShadingRate shadingRate) {
			InsertCommand<CESetShadingRateRenderCommand>(shadingRate);
		}

		void SetShadingRateImage(CETexture2D* shadingRateImage) {
			if (shadingRateImage) {
				shadingRateImage->AddRef();
			}
			InsertCommand<CESetShadingRateImageRenderCommand>(shadingRateImage);
		}

		void BeginRenderPass(CERenderTargetView* renderTargetView, CEDepthStencilView* depthStencilView,
		                     const Math::CEColorF& color) {
			Assert(renderTargetView != nullptr && depthStencilView != nullptr);
			renderTargetView->AddRef();
			depthStencilView->AddRef();
			InsertCommand<CEBeginRenderPassRenderCommand>(renderTargetView, depthStencilView, color);
		}

		void EndRenderPass() {
			InsertCommand<CEEndRenderPassRenderCommand>();
		}

		void SetViewport(float width, float height, float minDepth, float maxDepth, float x, float y) {
			InsertCommand<CESetViewportRenderCommand>(width, height, minDepth, maxDepth, x, y);
		}

		void SetScissorRect(float width, float height, float x, float y) {
			InsertCommand<CESetScissorRectRenderCommand>(width, height, x, y);
		}

		void SetBlendFactor(const Math::CEColorF& color) {
			InsertCommand<CESetBlendFactorRenderCommand>(color);
		}

		void SetRenderTargets(CERenderTargetView* const* renderTargetViews, uint32 renderTargetCount,
		                      CEDepthStencilView* depthStencilView) {
			CERenderTargetView** renderTargets = new(CommandAllocator) CERenderTargetView*[renderTargetCount];
			for (uint32 i = 0; i < renderTargetCount; i++) {
				renderTargets[i] = renderTargetViews[i];
				if (renderTargets[i]) {
					renderTargets[i]->AddRef();
				}
			}

			if (depthStencilView) {
				depthStencilView->AddRef();
			}
			InsertCommand<CESetRenderTargetsRenderCommand>(renderTargets, renderTargetCount, depthStencilView);
		}

		void SetPrimitiveTopology(CEPrimitiveTopology primitiveTopologyType) {
			InsertCommand<CESetPrimitiveTopologyRenderCommand>(primitiveTopologyType);
		}

		void SetVertexBuffers(CEVertexBuffer* const* vertexBuffers, uint32 vertexBufferCount, uint32 bufferSlot) {
			CEVertexBuffer** buffers = new(CommandAllocator) CEVertexBuffer*[vertexBufferCount];
			for (uint32 i = 0; i < vertexBufferCount; i++) {
				buffers[i] = vertexBuffers[i];
				if (buffers[i]) {
					buffers[i]->AddRef();
				}
			}
			InsertCommand<CESetVertexBuffersRenderCommand>(buffers, vertexBufferCount, bufferSlot);
		}

		void SetIndexBuffer(CEIndexBuffer* indexBuffer) {
			if (indexBuffer) {
				indexBuffer->AddRef();
			}
			InsertCommand<CESetIndexBufferRenderCommand>(indexBuffer);
		}

		void SetRayTracingBindings(
			CERayTracingScene* rayTracingScene,
			CERayTracingPipelineState* pipelineState,
			const CERayTracingShaderResources* globalResource,
			const CERayTracingShaderResources* rayGenLocalResources,
			const CERayTracingShaderResources* missLocalResources,
			const CERayTracingShaderResources* hitGroupResources,
			uint32 numHitGroupResources
		) {
			if (rayTracingScene) {
				rayTracingScene->AddRef();
			}
			if (pipelineState) {
				pipelineState->AddRef();
			}
			InsertCommand<CESetRayTracingBindingsRenderCommand>(
				rayTracingScene,
				pipelineState,
				globalResource,
				rayGenLocalResources,
				missLocalResources,
				hitGroupResources,
				numHitGroupResources
			);
		}

		void SetGraphicsPipelineState(CEGraphicsPipelineState* pipelineState) {
			if (pipelineState) {
				pipelineState->AddRef();
			}
			InsertCommand<CESetGraphicsPipelineStateRenderCommand>(pipelineState);
		}

		void SetComputePipelineState(CEComputePipelineState* pipelineState) {
			if (pipelineState) {
				pipelineState->AddRef();
			}
			InsertCommand<CESetComputePipelineStateRenderCommand>(pipelineState);
		}

		void Set32BitShaderConstants(CEShader* shader, const void* shader32BitConstants, uint32 num32BitConstants) {
			const uint32 num32BitConstantsInBytes = num32BitConstants * 4;
			void* shader32BitConstantsMemory = CommandAllocator.Allocate(num32BitConstantsInBytes, 1);
			Memory::CEMemory::Memcpy(shader32BitConstantsMemory, shader32BitConstants, num32BitConstantsInBytes);
		}

		void SetShaderResourceView(CEShader* shader, CEShaderResourceView* shaderResourceView, uint32 parameterIndex) {
			if (shader) {
				shader->AddRef();
			}
			if (shaderResourceView) {
				shaderResourceView->AddRef();
			}
			InsertCommand<CESetShaderResourceViewRenderCommand>(shader, shaderResourceView, parameterIndex);
		}

		void SetShaderResourceViews(CEShader* shader, CEShaderResourceView* const* shaderResourceViews,
		                            uint32 numShaderResourceViews, uint32 parameterIndex) {
			if (shader) {
				shader->AddRef();
			}
			CEShaderResourceView** tempShaderResourceViews = new(CommandAllocator) CEShaderResourceView*[
				numShaderResourceViews];
			for (uint32 i = 0; i < numShaderResourceViews; i++) {
				tempShaderResourceViews[i] = shaderResourceViews[i];
				if (tempShaderResourceViews[i]) {
					tempShaderResourceViews[i]->AddRef();
				}
			}
			InsertCommand<CESetShaderResourceViewsRenderCommand>(shader, tempShaderResourceViews,
			                                                     numShaderResourceViews, parameterIndex);
		}

		void SetUnorderedAccessView(CEShader* shader, CEUnorderedAccessView* unorderedAccessView,
		                            uint32 parameterIndex) {
			if (shader) {
				shader->AddRef();
			}
			if (unorderedAccessView) {
				unorderedAccessView->AddRef();
			}
			InsertCommand<CESetUnorderedAccessViewRenderCommand>(shader, unorderedAccessView, parameterIndex);
		}

		void SetUnorderedAccessViews(CEShader* shader, CEUnorderedAccessView* const* unorderedAccessViews,
		                             uint32 numUnorderedAccessViews, uint32 parameterIndex) {
			CEUnorderedAccessView** tempUnorderedAccessViews = new(CommandAllocator) CEUnorderedAccessView*[
				numUnorderedAccessViews];
			for (uint32 i = 0; i < numUnorderedAccessViews; i++) {
				tempUnorderedAccessViews[i] = unorderedAccessViews[i];
				if (tempUnorderedAccessViews[i]) {
					tempUnorderedAccessViews[i]->AddRef();
				}
			}
			if (shader) {
				shader->AddRef();
			}
			InsertCommand<CESetUnorderedAccessViewsRenderCommand>(shader, tempUnorderedAccessViews,
			                                                      numUnorderedAccessViews, parameterIndex);
		}

		void SetConstantBuffer(CEShader* shader, CEConstantBuffer* constantBuffer, uint32 parameterIndex) {
			if (shader) {
				shader->AddRef();
			}
			if (constantBuffer) {
				constantBuffer->AddRef();
			}
			InsertCommand<CESetConstantBufferRenderCommand>(shader, constantBuffer, parameterIndex);
		}

		void SetConstantBuffers(CEShader* shader, CEConstantBuffer* const* constantBuffers, uint32 numConstantBuffers,
		                        uint32 parameterIndex) {
			CEConstantBuffer** tempConstantBuffers = new(CommandAllocator) CEConstantBuffer*[numConstantBuffers];
			for (uint32 i = 0; i < numConstantBuffers; i++) {
				tempConstantBuffers[i] = constantBuffers[i];
				if (tempConstantBuffers[i]) {
					tempConstantBuffers[i]->AddRef();
				}
			}

			if (shader) {
				shader->AddRef();
			}
			InsertCommand<CESetConstantBuffersRenderCommand>(shader, tempConstantBuffers, numConstantBuffers,
			                                                 parameterIndex);
		}

		void SetSamplerState(CEShader* shader, CESamplerState* samplerState, uint32 parameterIndex) {
			if (shader) {
				shader->AddRef();
			}
			if (samplerState) {
				samplerState->AddRef();
			}
			InsertCommand<CESetSamplerStateRenderCommand>(shader, samplerState, parameterIndex);
		}

		void SetSamplerStates(CEShader* shader, CESamplerState* const* samplerStates, uint32 numSamplerStates,
		                      uint32 parameterIndex) {
			CESamplerState** tempSamplerStates = new(CommandAllocator)CESamplerState*[numSamplerStates];
			for (uint32 i = 0; i < numSamplerStates; i++) {
				tempSamplerStates[i] = samplerStates[i];
				if (tempSamplerStates[i]) {
					tempSamplerStates[i]->AddRef();
				}
			}

			if (shader) {
				shader->AddRef();
			}
			InsertCommand<CESetSamplerStatesRenderCommand>(shader, tempSamplerStates, numSamplerStates, parameterIndex);
		}

		void ResolveTexture(CETexture* destination, CETexture* source) {
			if (destination) {
				destination->AddRef();
			}
			if (source) {
				source->AddRef();
			}
			InsertCommand<CEResolveTextureRenderCommand>(destination, source);
		}

		void UpdateBuffer(CEBuffer* destination, uint64 destinationOffsetInBytes, uint64 sizeInBytes,
		                  const void* sourceData) {
			void* tempSourceData = CommandAllocator.Allocate(sizeInBytes, 1);
			Memory::CEMemory::Memcpy(tempSourceData, sourceData, sizeInBytes);
			if (destination) {
				destination->AddRef();
			}
			InsertCommand<CEUpdateBufferRenderCommand>(destination, destinationOffsetInBytes, sizeInBytes,
			                                           tempSourceData);
		}

		void UpdateTexture2D(CETexture2D* destination, uint32 width, uint32 height, uint32 mipLevel,
		                     const void* sourceData) {
			Assert(destination != nullptr);
			const uint32 sizeInBytes = width * height * GetByteStrideFromFormat(destination->GetFormat());

			void* tempSourceData = CommandAllocator.Allocate(sizeInBytes, 1);
			Memory::CEMemory::Memcpy(tempSourceData, sourceData, sizeInBytes);
		}

		void CopyBuffer(CEBuffer* destination, CEBuffer* source, const CECopyBufferInfo& copyInfo) {
			if (destination) {
				destination->AddRef();
			}
			if (source) {
				source->AddRef();
			}
			InsertCommand<CECopyBufferRenderCommand>(destination, source, copyInfo);
		}

		void CopyTexture(CETexture* destination, CETexture* source) {
			if (destination) {
				destination->AddRef();
			}
			if (source) {
				source->AddRef();
			}
			InsertCommand<CECopyTextureRenderCommand>(destination, source);
		}

		void CopyTextureRegion(CETexture* destination, CETexture* source, const CECopyTextureInfo& copyTextureInfo) {
			if (destination) {
				destination->AddRef();
			}
			if (source) {
				source->AddRef();
			}
			InsertCommand<CECopyTextureRegionRenderCommand>(destination, source, copyTextureInfo);
		}

		void DiscardResource(CEResource* resource) {
			if (resource) {
				resource->AddRef();
			}
			InsertCommand<CEDiscardResourceRenderCommand>(resource);
		}

		void BuildRayTracingGeometry(CERayTracingGeometry* geometry, CEVertexBuffer* vertexBuffer,
		                             CEIndexBuffer* indexBuffer, bool update) {
			Assert(geometry != nullptr);
			Assert(!update || (update && geometry->GetFlags() & RayTracingStructureBuildFlag_AllowUpdate));

			if (geometry) {
				geometry->AddRef();
			}
			if (vertexBuffer) {
				vertexBuffer->AddRef();
			}
			if (indexBuffer) {
				indexBuffer->AddRef();
			}
			InsertCommand<CEBuildRayTracingGeometryRenderCommand>(geometry, vertexBuffer, indexBuffer, update);
		}

		void BuildRayTracingScene(CERayTracingScene* scene, const CERayTracingGeometryInstance* instances,
		                          uint32 numInstances, bool update) {
			Assert(scene != nullptr);
			Assert(!update || (update && scene->GetFlags() & RayTracingStructureBuildFlag_AllowUpdate));
			if (scene) {
				scene->AddRef();
			}
			InsertCommand<CEBuildRayTracingSceneRenderCommand>(scene, instances, numInstances, update);

		}

		void GenerateMips(CETexture* texture) {
			Assert(texture != nullptr);
			texture->AddRef();
			InsertCommand<CEGenerateMipsRenderCommand>(texture);
		}

		void TransitionTexture(CETexture* texture, CEResourceState beforeState, CEResourceState afterState) {
			Assert(texture != nullptr);
			if (beforeState != afterState) {
				texture->AddRef();
				InsertCommand<CETransitionTextureRenderCommand>(texture, beforeState, afterState);
			}
			else {
				CE_LOG_WARNING(
					"Texture '" + texture->GetName() + "' was transitioned with same before and afterState (" + ToString
					(beforeState) + ")")
			}
		}

		void TransitionBuffer(CEBuffer* buffer, CEResourceState beforeState, CEResourceState afterState) {
			Assert(buffer != nullptr);
			if (beforeState != afterState) {
				buffer->AddRef();
				InsertCommand<CETransitionBufferRenderCommand>(buffer, beforeState, afterState);
			}
		}

		void UnorderedAccessTextureBarrier(CETexture* texture) {
			Assert(texture != nullptr);
			texture->AddRef();
			InsertCommand<CEUnorderedAccessTextureBarrierRenderCommand>(texture);
		}

		void UnorderedAccessBufferBarrier(CEBuffer* buffer) {
			Assert(buffer != nullptr);
			buffer->AddRef();
			InsertCommand<CEUnorderedAccessBufferBarrierRenderCommand>(buffer);
		}

		void Draw(uint32 vertexCount, uint32 startVertexLocation) {
			InsertCommand<CEDrawRenderCommand>(vertexCount, startVertexLocation);
			NumDrawCalls++;
		}

		void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) {
			InsertCommand<CEDrawIndexedRenderCommand>(indexCount, startIndexLocation, baseVertexLocation);
			NumDrawCalls++;
		}

		void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation,
		                   uint32 startInstanceLocation) {
			InsertCommand<CEDrawInstancedRenderCommand>(vertexCountPerInstance, instanceCount, startVertexLocation,
			                                            startInstanceLocation);
			NumDrawCalls++;
		}

		void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation,
		                          uint32 baseVertexLocation, uint32 startInstanceLocation) {
			InsertCommand<CEDrawIndexedInstancedRenderCommand>(indexCountPerInstance, instanceCount, startIndexLocation,
			                                                   baseVertexLocation, startInstanceLocation);
			NumDrawCalls++;
		}

		void Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) {
			InsertCommand<CEDispatchComputeRenderCommand>(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
			NumDispatchCalls++;
		}

		void DispatchRays(CERayTracingScene* scene, CERayTracingPipelineState* pipelineState, uint32 width,
		                  uint32 height, uint32 depth) {
			if (scene) {
				scene->AddRef();
			}
			if (pipelineState) {
				pipelineState->AddRef();
			}
			InsertCommand<CEDispatchRayRenderCommand>(scene, pipelineState, width, height, depth);
		}

		void InsertCommandListMaker(const std::string& marker) {
			InsertCommand<CEInsertCommandListMakerRenderCommand>(marker);
		}

		void DebugBreak() {
			InsertCommand<CEDebugBreakRenderCommand>();
		}

		void BeginExternalCapture() {
			InsertCommand<CEBeginExternalCaptureRenderCommand>();
		}

		void EndExternalCapture() {
			InsertCommand<CEEndExternalCaptureRenderCommand>();
		}

		void Reset() {
			if (FirstCommand != nullptr) {
				CERenderCommand* command = FirstCommand;
				while (command != nullptr) {
					CERenderCommand* oldCommand = command;
					command = command->NextCommand;
					oldCommand->~CERenderCommand();
				}
				FirstCommand = nullptr;
				LastCommand = nullptr;
			}

			NumDrawCalls = 0;
			NumDispatchCalls = 0;
			NumCommands = 0;

			CommandAllocator.Reset();
		}

		uint32 GetNumDrawCalls() const {
			return NumDrawCalls;
		}

		uint32 GetNumDispatchCalls() const {
			return NumDispatchCalls;
		}

		uint32 GetNumCommands() const {
			return NumCommands;
		}

	private:
		template <typename TCommand, typename... TArgs>
		void InsertCommand(TArgs&&... Args) {
			TCommand* Cmd = new(CommandAllocator) TCommand(Core::Containers::Forward<TArgs>(Args)...);
			if (LastCommand) {
				LastCommand->NextCommand = Cmd;
				LastCommand = LastCommand->NextCommand;
			}
			else {
				FirstCommand = Cmd;
				LastCommand = FirstCommand;
			}
			NumCommands++;
		}

	protected:
	private:
		Memory::CELinearAllocator CommandAllocator;
		CERenderCommand* FirstCommand;
		CERenderCommand* LastCommand;

		uint32 NumDrawCalls = 0;
		uint32 NumDispatchCalls = 0;
		uint32 NumCommands = 0;

		bool IsRecording = false;
	};

	class CECommandListExecutor {
	public:
		void ExecuteCommandList(CECommandList& commandList);
		void WaitForGPU();

		void SetContext(CEICommandContext* commandContext) {
			Assert(commandContext != nullptr);
			CommandContext = commandContext;
		}

		CEICommandContext& GetContext() {
			Assert(CommandContext != nullptr);
			return *CommandContext;
		}

	protected:
	private:
		CEICommandContext* CommandContext = nullptr;
	};
}

extern ConceptEngine::Graphics::Main::RenderLayer::CECommandListExecutor CommandListExecutor;
