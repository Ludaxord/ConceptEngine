#pragma once
#include "CEICommandContext.h"
#include "CERendering.h"
#include "CEResourceViews.h"
#include "CESamplerState.h"
#include "CEShader.h"
#include "CETexture.h"
#include "../../../Core/Common/CERef.h"
#include "../../../Math/CEColor.h"

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
		CEBeginTimeStampRenderCommand(CEGPUProfiler* profiler, uint32 index): Profiler(profiler), Index(index) {
		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.BeginTimeStamp(Profiler.Get(), Index);
		}

		Core::Common::CERef<CEGPUProfiler> Profiler;
		uint32 Index;
	};

	struct CEEndTimeStampRenderCommand : public CERenderCommand {
		CEEndTimeStampRenderCommand(CEGPUProfiler* profiler, uint32 index) : Profiler(profiler), Index(index) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.EndTimeStamp(Profiler.Get(), Index);
		}

		Core::Common::CERef<CEGPUProfiler> Profiler;
		uint32 Index;
	};

	struct CEClearRenderTargetViewRenderCommand : public CERenderCommand {
		CEClearRenderTargetViewRenderCommand(CERenderTargetView* renderTargetView, const Math::CEColorF& clearColor):
			RenderTargetView(renderTargetView), ClearColor(clearColor) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.ClearRenderTargetView(RenderTargetView.Get(), ClearColor);
		}

		Core::Common::CERef<CERenderTargetView> RenderTargetView;
		Math::CEColorF ClearColor;
	};

	struct CEClearDepthStencilViewRenderCommand : public CERenderCommand {
		CEClearDepthStencilViewRenderCommand(CEDepthStencilView* depthStencilView, const CEDepthStencilF& clearValue) :
			DepthStencilView(depthStencilView), ClearValue(clearValue) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.ClearDepthStencilView(DepthStencilView.Get(), ClearValue);
		};

		Core::Common::CERef<CEDepthStencilView> DepthStencilView;
		CEDepthStencilF ClearValue;
	};

	struct CEClearUnorderedAccessViewFloatRenderCommand : public CERenderCommand {
		CEClearUnorderedAccessViewFloatRenderCommand(CEUnorderedAccessView* unorderedAccessView,
		                                             const Math::CEColorF& clearColor) :
			UnorderedAccessView(unorderedAccessView), ClearColor(clearColor) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.ClearUnorderedAccessViewFloat(UnorderedAccessView.Get(), ClearColor);
		}

		Core::Common::CERef<CEUnorderedAccessView> UnorderedAccessView;
		Math::CEColorF ClearColor;
	};

	struct CESetShadingRateRenderCommand : public CERenderCommand {
		CESetShadingRateRenderCommand(CEShadingRate shadingRate): ShadingRate(shadingRate) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShadingRate(ShadingRate);
		}

		CEShadingRate ShadingRate;
	};

	struct CESetShadingRateImageRenderCommand : public CERenderCommand {
		CESetShadingRateImageRenderCommand(CETexture2D* shadingImage) : ShadingImage(shadingImage) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShadingRateImage(ShadingImage.Get());
		}

		Core::Common::CERef<CETexture2D> ShadingImage;

	};

	struct CESetViewportRenderCommand : public CERenderCommand {
		CESetViewportRenderCommand(float width, float height, float minDepth, float maxDepth, float x, float y):
			Width(width), Height(height), MinDepth(minDepth), MaxDepth(maxDepth), X(x), Y(y) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetViewport(Width, Height, MinDepth, MaxDepth, X, Y);
		}

		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
		float X;
		float Y;
	};

	struct CESetScissorRectRenderCommand : public CERenderCommand {
		CESetScissorRectRenderCommand(float width, float height, float x, float y): Width(width), Height(height), X(x),
			Y(y) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetScissorRect(Width, Height, X, Y);
		}

		float Width;
		float Height;
		float X;
		float Y;
	};

	struct CESetBlendFactorRenderCommand : public CERenderCommand {
		CESetBlendFactorRenderCommand(const Math::CEColorF& color) : Color(color) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetBlendFactor(Color);
		}

		Math::CEColorF Color;

	};

	struct CEBeginRenderPassRenderCommand : public CERenderCommand {
		CEBeginRenderPassRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.BeginRenderPass();
		}
	};

	struct CEEndRenderPassRenderCommand : public CERenderCommand {
		CEEndRenderPassRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.EndRenderPass();
		}
	};

	struct CESetPrimitiveTopologyRenderCommand : public CERenderCommand {
		CESetPrimitiveTopologyRenderCommand(CEPrimitiveTopology primitiveTopologyType): PrimitiveTopologyType(
			primitiveTopologyType) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetPrimitiveTopology(PrimitiveTopologyType);
		}

		CEPrimitiveTopology PrimitiveTopologyType;
	};

	struct CESetVertexBuffersRenderCommand : public CERenderCommand {
		CESetVertexBuffersRenderCommand(CEVertexBuffer** vertexBuffers, uint32 vertexBufferCount, uint32 startSlot) :
			VertexBuffers(vertexBuffers), VertexBufferCount(vertexBufferCount), StartSlot(startSlot) {

		}

		~CESetVertexBuffersRenderCommand() override {
			for (uint32 i = 0; i < VertexBufferCount; i++) {
				if (VertexBuffers[i]) {
					VertexBuffers[i]->Release();
					VertexBuffers[i] = nullptr;
				}
			}
			VertexBuffers = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetVertexBuffers(VertexBuffers, VertexBufferCount, StartSlot);
		}

		CEVertexBuffer** VertexBuffers;
		uint32 VertexBufferCount;
		uint32 StartSlot;
	};

	struct CESetIndexBufferRenderCommand : public CERenderCommand {
		CESetIndexBufferRenderCommand(CEIndexBuffer* indexBuffer): IndexBuffer(indexBuffer) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetIndexBuffer(IndexBuffer.Get());
		}

		Core::Common::CERef<CEIndexBuffer> IndexBuffer;
	};

	struct CESetRenderTargetsRenderCommand : public CERenderCommand {
		CESetRenderTargetsRenderCommand(CERenderTargetView** renderTargetViews, uint32 renderTargetViewCount,
		                                CEDepthStencilView* depthStencilView) : RenderTargetViews(renderTargetViews),
			RenderTargetViewCount(renderTargetViewCount), DepthStencilView(depthStencilView) {

		}

		~CESetRenderTargetsRenderCommand() {
			for (uint32 i = 0; i < RenderTargetViewCount; i++) {
				if (RenderTargetViews[i]) {
					RenderTargetViews[i]->Release();
					RenderTargetViews[i] = nullptr;
				}
			}
			RenderTargetViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetRenderTargets(RenderTargetViews, RenderTargetViewCount, DepthStencilView.Get());
		}

		CERenderTargetView** RenderTargetViews;
		uint32 RenderTargetViewCount;
		Core::Common::CERef<CEDepthStencilView> DepthStencilView;
	};

	struct CESetRayTracingBindingsRenderCommand : public CERenderCommand {
		CESetRayTracingBindingsRenderCommand(CERayTracingScene* rayTracingScene,
		                                     CERayTracingPipelineState* pipelineState,
		                                     const CERayTracingShaderResources* globalResources,
		                                     const CERayTracingShaderResources* rayGenLocalResources,
		                                     const CERayTracingShaderResources* missLocalResources,
		                                     const CERayTracingShaderResources* hitGroupResources,
		                                     uint32 numHitGroupResources
		)
			: Scene(rayTracingScene),
			  PipelineState(pipelineState),
			  GlobalResources(globalResources),
			  RayGenLocalResources(rayGenLocalResources),
			  MissLocalResources(missLocalResources),
			  HitGroupResources(hitGroupResources),
			  NumHitGroupResources(numHitGroupResources) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetRayTracingBindings(
				Scene.Get(),
				PipelineState.Get(),
				GlobalResources,
				RayGenLocalResources,
				MissLocalResources,
				HitGroupResources,
				NumHitGroupResources
			);
		}

		Core::Common::CERef<CERayTracingScene> Scene;
		Core::Common::CERef<CERayTracingPipelineState> PipelineState;
		Core::Common::CERef<CERayTracingShaderResources> GlobalResources;
		Core::Common::CERef<CERayTracingShaderResources> RayGenLocalResources;
		Core::Common::CERef<CERayTracingShaderResources> MissLocalResources;
		Core::Common::CERef<CERayTracingShaderResources> HitGroupResources;
		uint32 NumHitGroupResources;
	};

	struct CESetGraphicsPipelineStateRenderCommand : public CERenderCommand {
		CESetGraphicsPipelineStateRenderCommand(CEGraphicsPipelineState* pipelineState) : PipelineState(pipelineState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetGraphicsPipelineState(PipelineState.Get());
		}

		Core::Common::CERef<CEGraphicsPipelineState> PipelineState;
	};

	struct CESetComputePipelineStateRenderCommand : public CERenderCommand {
		CESetComputePipelineStateRenderCommand(CEComputePipelineState* pipelineState): PipelineState(pipelineState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetComputePipelineState(PipelineState.Get());
		}

		Core::Common::CERef<CEComputePipelineState> PipelineState;
	};

	struct CESet32BitShaderConstantsRenderCommand : public CERenderCommand {
		CESet32BitShaderConstantsRenderCommand(CEShader* shader, const void* shader32BitConstants,
		                                       uint32 num32BitConstants) : Shader(shader),
		                                                                   Shader32BitConstants(shader32BitConstants),
		                                                                   Num32BitConstants(num32BitConstants) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.Set32BitShaderConstants(Shader.Get(), Shader32BitConstants, Num32BitConstants);
		}

		Core::Common::CERef<CEShader> Shader;
		const void* Shader32BitConstants;
		uint32 Num32BitConstants;
	};

	struct CESetShaderResourceViewRenderCommand : public CERenderCommand {
		CESetShaderResourceViewRenderCommand(CEShader* shader, CEShaderResourceView* shaderResourceView,
		                                     uint32 parameterIndex): Shader(shader),
		                                                             ShaderResourceView(shaderResourceView),
		                                                             ParameterIndex(parameterIndex) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShaderResourceView(Shader.Get(), ShaderResourceView.Get(), ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		Core::Common::CERef<CEShaderResourceView> ShaderResourceView;
		uint32 ParameterIndex;
	};

	struct CESetShaderResourceViewsRenderCommand : public CERenderCommand {
		CESetShaderResourceViewsRenderCommand(CEShader* shader, CEShaderResourceView** shaderResourceViews,
		                                      uint32 numShaderResourceViews, uint32 parameterIndex): Shader(shader),
			ShaderResourceViews(shaderResourceViews), NumShaderResourceViews(numShaderResourceViews),
			ParameterIndex(parameterIndex) {

		}

		~CESetShaderResourceViewsRenderCommand() {
			for (uint32 i = 0; i < NumShaderResourceViews; i++) {
				if (ShaderResourceViews[i]) {
					ShaderResourceViews[i]->Release();
					ShaderResourceViews[i] = nullptr;
				}
			}
			ShaderResourceViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShaderResourceViews(Shader.Get(), ShaderResourceViews, NumShaderResourceViews,
			                                      ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		CEShaderResourceView** ShaderResourceViews;
		uint32 NumShaderResourceViews;
		uint32 ParameterIndex;
	};

	struct CESetUnorderedAccessViewRenderCommand : public CERenderCommand {
		CESetUnorderedAccessViewRenderCommand(CEShader* shader, CEUnorderedAccessView* unorderedAccessView,
		                                      uint32 parameterIndex): Shader(shader),
		                                                              UnorderedAccessView(unorderedAccessView),
		                                                              ParameterIndex(parameterIndex) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetUnorderedAccessView(Shader.Get(), UnorderedAccessView.Get(), ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		Core::Common::CERef<CEUnorderedAccessView> UnorderedAccessView;
		uint32 ParameterIndex;
	};

	struct CESetUnorderedAccessViewsRenderCommand : public CERenderCommand {
		CESetUnorderedAccessViewsRenderCommand(CEShader* shader, CEUnorderedAccessView** unorderedAccessViews,
		                                       uint32 numUnorderedAccessViews, uint32 parametersIndex):
			Shader(shader), UnorderedAccessViews(unorderedAccessViews),
			NumUnorderedAccessViews(numUnorderedAccessViews),
			ParametersIndex(parametersIndex) {

		}

		~CESetUnorderedAccessViewsRenderCommand() {
			for (uint32 i = 0; i < NumUnorderedAccessViews; i ++) {
				if (UnorderedAccessViews[i]) {
					UnorderedAccessViews[i]->Release();
					UnorderedAccessViews[i] = nullptr;
				}
			}

			UnorderedAccessViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetUnorderedAccessViews(Shader.Get(), UnorderedAccessViews, NumUnorderedAccessViews,
			                                       ParametersIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		CEUnorderedAccessView** UnorderedAccessViews;
		uint32 NumUnorderedAccessViews;
		uint32 ParametersIndex;
	};

	struct CESetConstantBufferRenderCommand : public CERenderCommand {
		CESetConstantBufferRenderCommand(CEShader* shader, CEConstantBuffer* constantBuffer, uint32 parameterIndex) :
			Shader(shader), ConstantBuffer(constantBuffer), ParameterIndex(parameterIndex) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetConstantBuffer(Shader.Get(), ConstantBuffer.Get(), ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		Core::Common::CERef<CEConstantBuffer> ConstantBuffer;
		uint32 ParameterIndex;
	};

	struct CESetConstantBuffersRenderCommand : public CERenderCommand {
		CESetConstantBuffersRenderCommand(CEShader* shader, CEConstantBuffer** constantBuffers,
		                                  uint32 numConstantBuffers, uint32 parameterIndex) {

		}

		~CESetConstantBuffersRenderCommand() {
			for (uint32 i = 0; i < NumConstantBuffers; i++) {
				if (ConstantBuffers[i]) {
					ConstantBuffers[i]->Release();
					ConstantBuffers[i] = nullptr;
				}
			}

			ConstantBuffers = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetConstantBuffers(Shader.Get(), ConstantBuffers, NumConstantBuffers, ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		CEConstantBuffer** ConstantBuffers;
		uint32 NumConstantBuffers;
		uint32 ParameterIndex;

	};

	struct CESetSamplerStateRenderCommand : public CERenderCommand {
		CESetSamplerStateRenderCommand(CEShader* shader, CESamplerState* samplerState, uint32 parameterIndex) :
			Shader(shader), SamplerState(samplerState), ParameterIndex(parameterIndex) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetSamplerState(Shader.Get(), SamplerState.Get(), ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		Core::Common::CERef<CESamplerState> SamplerState;
		uint32 ParameterIndex;
	};

	struct CESetSamplerStatesRenderCommand : public CERenderCommand {
		CESetSamplerStatesRenderCommand(CEShader* shader, CESamplerState** samplerStates, uint32 numSamplerStates,
		                                uint32 parameterIndex) : Shader(shader), SamplerStates(samplerStates),
		                                                         NumSamplerStates(numSamplerStates),
		                                                         ParameterIndex(parameterIndex) {

		}

		~CESetSamplerStatesRenderCommand() {
			for (uint32 i = 0; i < NumSamplerStates; i++) {
				if (SamplerStates[i]) {
					SamplerStates[i]->Release();
					SamplerStates[i] = nullptr;
				}
			}

			SamplerStates = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetSamplerStates(Shader.Get(), SamplerStates, NumSamplerStates, ParameterIndex);
		}

		Core::Common::CERef<CEShader> Shader;
		CESamplerState** SamplerStates;
		uint32 NumSamplerStates;
		uint32 ParameterIndex;

	};

	struct CEResolveTextureRenderCommand : public CERenderCommand {
		CEResolveTextureRenderCommand(CETexture* destination, CETexture* source) : Destination(destination),
			Source(source) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.ResolveTexture(Destination.Get(), Source.Get());
		}

		Core::Common::CERef<CETexture> Destination;
		Core::Common::CERef<CETexture> Source;
	};

	struct CEUpdateBufferRenderCommand : public CERenderCommand {
		CEUpdateBufferRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEUpdateTexture2DRenderCommand : public CERenderCommand {
		CEUpdateTexture2DRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CECopyBufferRenderCommand : public CERenderCommand {
		CECopyBufferRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CECopyTextureRenderCommand : public CERenderCommand {
		CECopyTextureRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDiscardResourceRenderCommand : public CERenderCommand {
		CEDiscardResourceRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEBuildRayTracingGeometryRenderCommand : public CERenderCommand {
		CEBuildRayTracingGeometryRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEBuildRayTracingSceneRenderCommand : public CERenderCommand {
		CEBuildRayTracingSceneRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEGenerateMipsRenderCommand : public CERenderCommand {
		CEGenerateMipsRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CETransitionTextureRenderCommand : public CERenderCommand {
		CETransitionTextureRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CETransitionBufferRenderCommand : public CERenderCommand {
		CETransitionBufferRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEUnorderedAccessTextureBarrierRenderCommand : public CERenderCommand {
		CEUnorderedAccessTextureBarrierRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEUnorderedAccessBufferBarrierRenderCommand : public CERenderCommand {
		CEUnorderedAccessBufferBarrierRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDrawRenderCommand : public CERenderCommand {
		CEDrawRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDrawIndexedRenderCommand : public CERenderCommand {
		CEDrawIndexedRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDrawInstancedRenderCommand : public CERenderCommand {
		CEDrawInstancedRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDrawIndexedInstancedRenderCommand : public CERenderCommand {
		CEDrawIndexedInstancedRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDispatchComputeRenderCommand : public CERenderCommand {
		CEDispatchComputeRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDispatchRayRenderCommand : public CERenderCommand {
		CEDispatchRayRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEInsertCommandListMakerRenderCommand : public CERenderCommand {
		CEInsertCommandListMakerRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEDebugBreakRenderCommand : public CERenderCommand {
		CEDebugBreakRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEBeginExternalCaptureRenderCommand : public CERenderCommand {
		CEBeginExternalCaptureRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};

	struct CEEndExternalCaptureRenderCommand : public CERenderCommand {
		CEEndExternalCaptureRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {

		}
	};
}
