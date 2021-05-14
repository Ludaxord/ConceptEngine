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

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CECommandList {
	public:
		CECommandList() : CommandAllocator(), FirstCommand(nullptr), LastCommand(nullptr) {

		}

		~CECommandList() {
			Reset();
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

		void BeginRenderPass() {
			InsertCommand<CEBeginRenderPassRenderCommand>();
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

		void SetUnorderedAccessView() {

		}

		void SetUnorderedAccessViews() {

		}

		void SetConstantBuffer() {
		}

		void SetConstantBuffers() {

		}

		void SetSamplerState() {

		}

		void SetSamplerStates() {

		}

		void ResolveTexture() {

		}

		void UpdateBuffer() {

		}

		void UpdateTexture2D() {

		}

		void CopyBuffer() {

		}

		void CopyTexture() {

		}

		void CopyTextureRegion() {

		}

		void DiscardResource() {

		}

		void BuildRayTracingGeometry() {

		}

		void BuildRayTracingScene() {

		}

		void GenerateMips() {

		}

		void TransitionTexture() {

		}

		void TransitionBuffer() {

		}

		void UnorderedAccessTextureBarrier() {

		}

		void UnorderedAccessBufferBarrier() {

		}

		void Draw() {

		}

		void DrawIndexed() {

		}

		void DrawInstanced() {

		}

		void DrawIndexedInstanced() {

		}

		void Dispatch() {
		}

		void DispatchRays() {
		}

		void InsertCommandListMaker() {

		}

		void DebugBreak() {
		}

		void BeginExternalCapture() {

		}

		void EndExternalCapture() {

		}

		void Reset() {

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
