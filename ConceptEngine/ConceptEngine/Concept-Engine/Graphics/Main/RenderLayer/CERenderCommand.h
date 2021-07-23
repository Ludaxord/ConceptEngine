#pragma once
#include <string>

#include "CEICommandContext.h"
#include "CERayTracing.h"
#include "CERendering.h"
#include "CEResourceViews.h"
#include "CESamplerState.h"
#include "CEShader.h"
#include "CETexture.h"
#include "../../../Core/Common/CERef.h"
#include "../../../Core/Common/CETypes.h"
#include "../../../Math/CEColor.h"
#include "../../../Core/Log/CELog.h"
#include "../../../Core/Debug/CEDebug.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	struct CERenderCommand {
		virtual ~CERenderCommand() = default;

		virtual void Execute(CEICommandContext&) = 0;

		void operator()(CEICommandContext& commandContext) {
			Execute(commandContext);
		}

		virtual std::string GetName() = 0;

		CERenderCommand* NextCommand = nullptr;
	};

	struct CEBeginRenderCommand : public CERenderCommand {
		CEBeginRenderCommand() {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.Begin();
		}

		std::string GetName() override {
			return "CEBeginRenderCommand";
		};
	};

	struct CEEndRenderCommand : public CERenderCommand {
		CEEndRenderCommand() {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.End();
		}

		std::string GetName() override {
			return "CEEndRenderCommand";
		};
	};

	struct CEBeginTimeStampRenderCommand : public CERenderCommand {
		CEBeginTimeStampRenderCommand(CEGPUProfiler* profiler, uint32 index): Profiler(profiler), Index(index) {
		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.BeginTimeStamp(Profiler.Get(), Index);
		}

		std::string GetName() override {
			return "CEBeginTimeStampRenderCommand";
		};

		CERef<CEGPUProfiler> Profiler;
		uint32 Index;
	};

	struct CEEndTimeStampRenderCommand : public CERenderCommand {
		CEEndTimeStampRenderCommand(CEGPUProfiler* profiler, uint32 index) : Profiler(profiler), Index(index) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.EndTimeStamp(Profiler.Get(), Index);
		}

		std::string GetName() override {
			return "CEEndTimeStampRenderCommand";
		};

		CERef<CEGPUProfiler> Profiler;
		uint32 Index;
	};

	struct CEClearRenderTargetViewRenderCommand : public CERenderCommand {
		CEClearRenderTargetViewRenderCommand(CERenderTargetView* renderTargetView, const Math::CEColorF& clearColor):
			RenderTargetView(renderTargetView), ClearColor(clearColor) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.ClearRenderTargetView(RenderTargetView.Get(), ClearColor);
		}

		std::string GetName() override {
			return "CEClearRenderTargetViewRenderCommand";
		};

		CERef<CERenderTargetView> RenderTargetView;
		Math::CEColorF ClearColor;
	};

	struct CEClearDepthStencilViewRenderCommand : public CERenderCommand {
		CEClearDepthStencilViewRenderCommand(CEDepthStencilView* depthStencilView, const CEDepthStencilF& clearValue) :
			DepthStencilView(depthStencilView), ClearValue(clearValue) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.ClearDepthStencilView(DepthStencilView.Get(), ClearValue);
		};

		std::string GetName() override {
			return "CEClearDepthStencilViewRenderCommand";
		};

		CERef<CEDepthStencilView> DepthStencilView;
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

		std::string GetName() override {
			return "CEClearUnorderedAccessViewFloatRenderCommand";
		};

		CERef<CEUnorderedAccessView> UnorderedAccessView;
		Math::CEColorF ClearColor;
	};

	struct CESetShadingRateRenderCommand : public CERenderCommand {
		CESetShadingRateRenderCommand(CEShadingRate shadingRate): ShadingRate(shadingRate) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShadingRate(ShadingRate);
		}

		std::string GetName() override {
			return "CESetShadingRateRenderCommand";
		};

		CEShadingRate ShadingRate;
	};

	struct CESetShadingRateImageRenderCommand : public CERenderCommand {
		CESetShadingRateImageRenderCommand(CETexture2D* shadingImage) : ShadingImage(shadingImage) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShadingRateImage(ShadingImage.Get());
		}

		std::string GetName() override {
			return "CESetShadingRateImageRenderCommand";
		};

		CERef<CETexture2D> ShadingImage;

	};

	struct CESetDebugPointCommand : public CERenderCommand {
		CESetDebugPointCommand(std::string Message) : Message(Message) {
		}

		virtual void Execute(CEICommandContext&) override {
			CE_LOG_DEBUGX("[DEBUG_POINT]: "+ Message)
		}

		std::string GetName() override {
			return "CESetDebugPointCommand";
		};

		std::string Message;
	};

	struct CESetViewportRenderCommand : public CERenderCommand {
		CESetViewportRenderCommand(float width, float height, float minDepth, float maxDepth, float x, float y):
			Width(width), Height(height), MinDepth(minDepth), MaxDepth(maxDepth), X(x), Y(y) {

		}

		virtual void Execute(CEICommandContext& commandContext) override {
			commandContext.SetViewport(Width, Height, MinDepth, MaxDepth, X, Y);
		}

		std::string GetName() override {
			return "CESetViewportRenderCommand";
		};

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

		std::string GetName() override {
			return "CESetScissorRectRenderCommand";
		};

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

		std::string GetName() override {
			return "CESetBlendFactorRenderCommand";
		};

		Math::CEColorF Color;

	};

	struct CEBeginRenderPassRenderCommand : public CERenderCommand {
		CEBeginRenderPassRenderCommand(CERenderTargetView* renderTargetView,
		                               CEDepthStencilView* depthStencilView,
		                               const Math::CEColorF& color): Color(color),
		                                                             RenderTargetView(renderTargetView),
		                                                             DepthStencilView(depthStencilView) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.BeginRenderPass(Color, RenderTargetView.Get(), DepthStencilView.Get());
		}

		std::string GetName() override {
			return "CEBeginRenderPassRenderCommand";
		};

		Math::CEColorF Color;
		CERef<CERenderTargetView> RenderTargetView;
		CERef<CEDepthStencilView> DepthStencilView;
	};

	struct CEEndRenderPassRenderCommand : public CERenderCommand {
		CEEndRenderPassRenderCommand() {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.EndRenderPass();
		}

		std::string GetName() override {
			return "CEEndRenderPassRenderCommand";
		};
	};

	struct CESetPrimitiveTopologyRenderCommand : public CERenderCommand {
		CESetPrimitiveTopologyRenderCommand(CEPrimitiveTopology primitiveTopologyType): PrimitiveTopologyType(
			primitiveTopologyType) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetPrimitiveTopology(PrimitiveTopologyType);
		}

		std::string GetName() override {
			return "CESetPrimitiveTopologyRenderCommand";
		};

		CEPrimitiveTopology PrimitiveTopologyType;
	};

	struct CESetVertexBuffersRenderCommand : public CERenderCommand {
		CESetVertexBuffersRenderCommand(CEVertexBuffer** vertexBuffers, uint32 vertexBufferCount, uint32 startSlot) :
			VertexBuffers(vertexBuffers), VertexBufferCount(vertexBufferCount), StartSlot(startSlot) {

		}

		~CESetVertexBuffersRenderCommand() {
			for (uint32 i = 0; i < VertexBufferCount; i++) {
				SafeRelease(VertexBuffers[i]);
			}
			VertexBuffers = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetVertexBuffers(VertexBuffers, VertexBufferCount, StartSlot);
		}

		std::string GetName() override {
			return "CESetVertexBuffersRenderCommand";
		};

		CEVertexBuffer** VertexBuffers;
		uint32 VertexBufferCount;
		uint32 StartSlot;
	};

	struct CESetIndexBufferRenderCommand : public CERenderCommand {
		CESetIndexBufferRenderCommand(CEIndexBuffer* indexBuffer): IndexBuffer(indexBuffer) {

		}

		void Execute(CEICommandContext& commandContext) override {
			if (IndexBuffer == nullptr) {
				CE_LOG_VERBOSE("INDEX BUFFER IS NULLPTR")
			}
			commandContext.SetIndexBuffer(IndexBuffer.Get());
		}

		std::string GetName() override {
			return "CESetIndexBufferRenderCommand";
		};

		CERef<CEIndexBuffer> IndexBuffer;
	};

	struct CESetRenderTargetsRenderCommand : public CERenderCommand {
		CESetRenderTargetsRenderCommand(CERenderTargetView** renderTargetViews, uint32 renderTargetViewCount,
		                                CEDepthStencilView* depthStencilView) : RenderTargetViews(renderTargetViews),
			RenderTargetViewCount(renderTargetViewCount), DepthStencilView(depthStencilView) {

		}

		~CESetRenderTargetsRenderCommand() {
			for (uint32 i = 0; i < RenderTargetViewCount; i++) {
				SafeRelease(RenderTargetViews[i]);
			}
			RenderTargetViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetRenderTargets(RenderTargetViews, RenderTargetViewCount, DepthStencilView.Get());
		}

		std::string GetName() override {
			return "CESetRenderTargetsRenderCommand";
		};

		CERenderTargetView** RenderTargetViews;
		uint32 RenderTargetViewCount;
		CERef<CEDepthStencilView> DepthStencilView;
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

		std::string GetName() override {
			return "CESetRayTracingBindingsRenderCommand";
		};

		CERef<CERayTracingScene> Scene;
		CERef<CERayTracingPipelineState> PipelineState;
		const CERayTracingShaderResources* GlobalResources;
		const CERayTracingShaderResources* RayGenLocalResources;
		const CERayTracingShaderResources* MissLocalResources;
		const CERayTracingShaderResources* HitGroupResources;
		uint32 NumHitGroupResources;
	};

	struct CESetGraphicsPipelineStateRenderCommand : public CERenderCommand {
		CESetGraphicsPipelineStateRenderCommand(CEGraphicsPipelineState* pipelineState) : PipelineState(pipelineState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetGraphicsPipelineState(PipelineState.Get());
		}

		std::string GetName() override {
			return "CESetGraphicsPipelineStateRenderCommand";
		};

		CERef<CEGraphicsPipelineState> PipelineState;
	};

	struct CESetComputePipelineStateRenderCommand : public CERenderCommand {
		CESetComputePipelineStateRenderCommand(CEComputePipelineState* pipelineState): PipelineState(pipelineState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetComputePipelineState(PipelineState.Get());
		}

		std::string GetName() override {
			return "CESetComputePipelineStateRenderCommand";
		};

		CERef<CEComputePipelineState> PipelineState;
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

		std::string GetName() override {
			return "CESet32BitShaderConstantsRenderCommand";
		};

		CERef<CEShader> Shader;
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

		std::string GetName() override {
			return "CESetShaderResourceViewRenderCommand";
		};

		CERef<CEShader> Shader;
		CERef<CEShaderResourceView> ShaderResourceView;
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
				SafeRelease(ShaderResourceViews[i]);
			}
			ShaderResourceViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetShaderResourceViews(Shader.Get(), ShaderResourceViews, NumShaderResourceViews,
			                                      ParameterIndex);
		}

		std::string GetName() override {
			return "CESetShaderResourceViewsRenderCommand";
		};

		CERef<CEShader> Shader;
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

		std::string GetName() override {
			return "CESetUnorderedAccessViewRenderCommand";
		};

		CERef<CEShader> Shader;
		CERef<CEUnorderedAccessView> UnorderedAccessView;
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
				SafeRelease(UnorderedAccessViews[i])
			}

			UnorderedAccessViews = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetUnorderedAccessViews(Shader.Get(), UnorderedAccessViews, NumUnorderedAccessViews,
			                                       ParametersIndex);
		}

		std::string GetName() override {
			return "CESetUnorderedAccessViewsRenderCommand";
		};

		CERef<CEShader> Shader;
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

		std::string GetName() override {
			return "CESetConstantBufferRenderCommand";
		};

		CERef<CEShader> Shader;
		CERef<CEConstantBuffer> ConstantBuffer;
		uint32 ParameterIndex;
	};

	struct CESetConstantBuffersRenderCommand : public CERenderCommand {
		CESetConstantBuffersRenderCommand(CEShader* shader, CEConstantBuffer** constantBuffers,
		                                  uint32 numConstantBuffers, uint32 parameterIndex)
			: Shader(shader)
			  , ConstantBuffers(constantBuffers)
			  , NumConstantBuffers(numConstantBuffers)
			  , ParameterIndex(parameterIndex) {

		}

		~CESetConstantBuffersRenderCommand() {
			for (uint32 i = 0; i < NumConstantBuffers; i++) {
				SafeRelease(ConstantBuffers[i]);
			}

			ConstantBuffers = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetConstantBuffers(Shader.Get(), ConstantBuffers, NumConstantBuffers, ParameterIndex);
		}

		std::string GetName() override {
			return "CESetConstantBuffersRenderCommand";
		};

		CERef<CEShader> Shader;
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

		std::string GetName() override {
			return "CESetSamplerStateRenderCommand";
		};

		CERef<CEShader> Shader;
		CERef<CESamplerState> SamplerState;
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
				SafeRelease(SamplerStates[i]);
			}

			SamplerStates = nullptr;
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.SetSamplerStates(Shader.Get(), SamplerStates, NumSamplerStates, ParameterIndex);
		}

		std::string GetName() override {
			return "CESetSamplerStatesRenderCommand";
		};

		CERef<CEShader> Shader;
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

		std::string GetName() override {
			return "CEResolveTextureRenderCommand";
		};

		CERef<CETexture> Destination;
		CERef<CETexture> Source;
	};

	struct CEUpdateBufferRenderCommand : public CERenderCommand {
		CEUpdateBufferRenderCommand(CEBuffer* destination, uint64 destinationOffsetInBytes, uint64 sizeInBytes,
		                            const void* sourceData): Destination(destination),
		                                                     DestinationOffsetInBytes(destinationOffsetInBytes),
		                                                     SizeInBytes(sizeInBytes), SourceData(sourceData) {
			Assert(sourceData != nullptr);
			Assert(sizeInBytes != 0);
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.UpdateBuffer(Destination.Get(), DestinationOffsetInBytes, SizeInBytes, SourceData);
		}

		std::string GetName() override {
			return "CEUpdateBufferRenderCommand";
		};

		CERef<CEBuffer> Destination;
		uint64 DestinationOffsetInBytes;
		uint64 SizeInBytes;
		const void* SourceData;
	};

	struct CEUpdateTexture2DRenderCommand : public CERenderCommand {
		CEUpdateTexture2DRenderCommand(CETexture2D* destination, uint32 width, uint32 height, uint32 mipLevel,
		                               const void* sourceData): Destination(destination), Width(width), Height(height),
		                                                        MipLevel(mipLevel), SourceData(sourceData) {
			Assert(sourceData != nullptr);
		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.UpdateTexture2D(Destination.Get(), Width, Height, MipLevel, SourceData);
		}

		std::string GetName() override {
			return "CEUpdateTexture2DRenderCommand";
		};

		CERef<CETexture2D> Destination;
		uint32 Width;
		uint32 Height;
		uint32 MipLevel;
		const void* SourceData;
	};

	struct CECopyBufferRenderCommand : public CERenderCommand {
		CECopyBufferRenderCommand(CEBuffer* destination, CEBuffer* source, const CECopyBufferInfo& copyBufferInfo) :
			Destination(destination), Source(source), CopyBufferInfo(copyBufferInfo) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.CopyBuffer(Destination.Get(), Source.Get(), CopyBufferInfo);
		}

		std::string GetName() override {
			return "CECopyBufferRenderCommand";
		};

		CERef<CEBuffer> Destination;
		CERef<CEBuffer> Source;
		CECopyBufferInfo CopyBufferInfo;
	};

	struct CECopyTextureRenderCommand : public CERenderCommand {
		CECopyTextureRenderCommand(CETexture* destination, CETexture* source) : Destination(destination),
			Source(source) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.CopyTexture(Destination.Get(), Source.Get());
		}

		std::string GetName() override {
			return "CECopyTextureRenderCommand";
		};

		CERef<CETexture> Destination;
		CERef<CETexture> Source;
	};

	struct CECopyTextureRegionRenderCommand : public CERenderCommand {
		CECopyTextureRegionRenderCommand(CETexture* destination, CETexture* source,
		                                 const CECopyTextureInfo& copyTextureInfo):
			Destination(destination), Source(source), CopyTextureInfo(copyTextureInfo) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.CopyTextureRegion(Destination.Get(), Source.Get(), CopyTextureInfo);

		}

		std::string GetName() override {
			return "CECopyTextureRegionRenderCommand";
		};

		CERef<CETexture> Destination;
		CERef<CETexture> Source;
		CECopyTextureInfo CopyTextureInfo;
	};

	struct CEDiscardResourceRenderCommand : public CERenderCommand {
		CEDiscardResourceRenderCommand(CEResource* resource):
			Resource(resource) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.DiscardResource(Resource.Get());
		}

		std::string GetName() override {
			return "CEDiscardResourceRenderCommand";
		};

		CERef<CEResource> Resource;
	};

	struct CEBuildRayTracingGeometryRenderCommand : public CERenderCommand {
		CEBuildRayTracingGeometryRenderCommand(CERayTracingGeometry* rayTracingGeometry, CEVertexBuffer* vertexBuffer,
		                                       CEIndexBuffer* indexBuffer, bool update) :
			RayTracingGeometry(rayTracingGeometry), VertexBuffer(vertexBuffer), IndexBuffer(indexBuffer),
			Update(update) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.BuildRayTracingGeometry(RayTracingGeometry.Get(), VertexBuffer.Get(), IndexBuffer.Get(),
			                                       Update);
		}

		std::string GetName() override {
			return "CEBuildRayTracingGeometryRenderCommand";
		};

		CERef<CERayTracingGeometry> RayTracingGeometry;
		CERef<CEVertexBuffer> VertexBuffer;
		CERef<CEIndexBuffer> IndexBuffer;
		bool Update;
	};

	struct CEBuildRayTracingSceneRenderCommand : public CERenderCommand {
		CEBuildRayTracingSceneRenderCommand(CERayTracingScene* rayTracingScene,
		                                    const CERayTracingGeometryInstance* instances, uint32 numInstances,
		                                    bool update): RayTracingScene(rayTracingScene),
		                                                  Instances(instances), NumInstances(numInstances),
		                                                  Update(update) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.BuildRayTracingScene(RayTracingScene.Get(), Instances, NumInstances, Update);
		}

		std::string GetName() override {
			return "CEBuildRayTracingSceneRenderCommand";
		};

		CERef<CERayTracingScene> RayTracingScene;
		const CERayTracingGeometryInstance* Instances;
		uint32 NumInstances;
		bool Update;
	};

	struct CEGenerateMipsRenderCommand : public CERenderCommand {
		CEGenerateMipsRenderCommand(CETexture* texture): Texture(texture) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.GenerateMips(Texture.Get());
		}

		std::string GetName() override {
			return "CEGenerateMipsRenderCommand";
		};

		CERef<CETexture> Texture;
	};

	struct CETransitionTextureRenderCommand : public CERenderCommand {
		CETransitionTextureRenderCommand(CETexture* texture, CEResourceState beforeState, CEResourceState afterState) :
			Texture(texture), BeforeState(beforeState), AfterState(afterState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.TransitionTexture(Texture.Get(), BeforeState, AfterState);
		}

		std::string GetName() override {
			return "CETransitionTextureRenderCommand";
		};

		CERef<CETexture> Texture;
		CEResourceState BeforeState;
		CEResourceState AfterState;
	};

	struct CETransitionBufferRenderCommand : public CERenderCommand {
		CETransitionBufferRenderCommand(CEBuffer* buffer, CEResourceState beforeState, CEResourceState afterState) :
			Buffer(buffer), BeforeState(beforeState), AfterState(afterState) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.TransitionBuffer(Buffer.Get(), BeforeState, AfterState);
		}

		std::string GetName() override {
			return "CETransitionBufferRenderCommand";
		};

		CERef<CEBuffer> Buffer;
		CEResourceState BeforeState;
		CEResourceState AfterState;
	};

	struct CEUnorderedAccessTextureBarrierRenderCommand : public CERenderCommand {
		CEUnorderedAccessTextureBarrierRenderCommand(CETexture* texture): Texture(texture) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.UnorderedAccessTextureBarrier(Texture.Get());
		}

		std::string GetName() override {
			return "CEUnorderedAccessTextureBarrierRenderCommand";
		};

		CERef<CETexture> Texture;
	};

	struct CEUnorderedAccessBufferBarrierRenderCommand : public CERenderCommand {
		CEUnorderedAccessBufferBarrierRenderCommand(CEBuffer* buffer): Buffer(buffer) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.UnorderedAccessBufferBarrier(Buffer.Get());
		}

		std::string GetName() override {
			return "CEUnorderedAccessBufferBarrierRenderCommand";
		};

		CERef<CEBuffer> Buffer;
	};

	struct CEDrawRenderCommand : public CERenderCommand {
		CEDrawRenderCommand(uint32 vertexCount, uint32 startVertexLocation) : VertexCount(vertexCount),
		                                                                      StartVertexLocation(startVertexLocation) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.Draw(VertexCount, StartVertexLocation);
		}

		std::string GetName() override {
			return "CEDrawRenderCommand";
		};

		uint32 VertexCount;
		uint32 StartVertexLocation;
	};

	struct CEDrawIndexedRenderCommand : public CERenderCommand {
		CEDrawIndexedRenderCommand(uint32 indexCount, uint32 startIndexLocation, int32 baseVertexLocation):
			IndexCount(indexCount), StartIndexLocation(startIndexLocation), BaseVertexLocation(baseVertexLocation) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		}

		std::string GetName() override {
			return "CEDrawIndexedRenderCommand";
		};

		uint32 IndexCount;
		uint32 StartIndexLocation;
		int32 BaseVertexLocation;
	};

	struct CEDrawInstancedRenderCommand : public CERenderCommand {
		CEDrawInstancedRenderCommand(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation,
		                             uint32 startInstanceLocation): VertexCountPerInstance(vertexCountPerInstance),
		                                                            InstanceCount(instanceCount),
		                                                            StartVertexLocation(startVertexLocation),
		                                                            StartInstanceLocation(startInstanceLocation) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation,
			                             StartInstanceLocation);
		}

		std::string GetName() override {
			return "CEDrawInstancedRenderCommand";
		};

		uint32 VertexCountPerInstance;
		uint32 InstanceCount;
		uint32 StartVertexLocation;
		uint32 StartInstanceLocation;
	};

	struct CEDrawIndexedInstancedRenderCommand : public CERenderCommand {
		CEDrawIndexedInstancedRenderCommand(uint32 indexCountPerInstance, uint32 instanceCount,
		                                    uint32 startIndexLocation, uint32 baseVertexLocation,
		                                    uint32 startInstanceLocation):
			IndexCountPerInstance(indexCountPerInstance),
			InstanceCount(instanceCount),
			StartIndexLocation(startIndexLocation),
			BaseVertexLocation(baseVertexLocation),
			StartInstanceLocation(startInstanceLocation) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation,
			                                    BaseVertexLocation, StartInstanceLocation);
		}

		std::string GetName() override {
			return "CEDrawIndexedInstancedRenderCommand";
		};

		uint32 IndexCountPerInstance;
		uint32 InstanceCount;
		uint32 StartIndexLocation;
		uint32 BaseVertexLocation;
		uint32 StartInstanceLocation;
	};

	struct CEDispatchComputeRenderCommand : public CERenderCommand {
		CEDispatchComputeRenderCommand(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ):
			ThreadGroupCountX(threadGroupCountX), ThreadGroupCountY(threadGroupCountY),
			ThreadGroupCountZ(threadGroupCountZ) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
		}

		std::string GetName() override {
			return "CEDispatchComputeRenderCommand";
		};

		uint32 ThreadGroupCountX;
		uint32 ThreadGroupCountY;
		uint32 ThreadGroupCountZ;
	};

	struct CEDispatchRayRenderCommand : public CERenderCommand {
		CEDispatchRayRenderCommand(CERayTracingScene* rayTracingScene, CERayTracingPipelineState* pipelineState,
		                           uint32 width, uint32 height, uint32 depth): Scene(rayTracingScene),
		                                                                       PipelineState(pipelineState),
		                                                                       Width(width), Height(height),
		                                                                       Depth(depth) {

		}

		void Execute(CEICommandContext& commandContext) override {
			commandContext.DispatchRays(Scene.Get(), PipelineState.Get(), Width, Height, Depth);
		}

		std::string GetName() override {
			return "CEDispatchRayRenderCommand";
		};

		CERef<CERayTracingScene> Scene;
		CERef<CERayTracingPipelineState> PipelineState;
		uint32 Width;
		uint32 Height;
		uint32 Depth;
	};

	struct CEInsertCommandListMarkerRenderCommand : public CERenderCommand {
		CEInsertCommandListMarkerRenderCommand(const std::string& marker): Marker(marker) {

		}

		void Execute(CEICommandContext& commandContext) override {
			CEDebug::OutputDebugString(Marker + '\n');
			CE_LOG_INFO(Marker);
			commandContext.InsertMaker(Marker);
		}

		std::string GetName() override {
			return "CEInsertCommandListMarkerRenderCommand";
		};

		std::string Marker;
	};

	struct CEDebugBreakRenderCommand : public CERenderCommand {

		void Execute(CEICommandContext& commandContext) override {
			(void)commandContext;
			CEDebug::DebugBreak();
		}

		std::string GetName() override {
			return "CEDebugBreakRenderCommand";
		};
	};

	struct CEBeginExternalCaptureRenderCommand : public CERenderCommand {

		void Execute(CEICommandContext& commandContext) override {
			commandContext.BeginExternalCapture();
		}

		std::string GetName() override {
			return "CEBeginExternalCaptureRenderCommand";
		};
	};

	struct CEEndExternalCaptureRenderCommand : public CERenderCommand {
		void Execute(CEICommandContext& commandContext) override {
			commandContext.EndExternalCapture();
		}

		std::string GetName() override {
			return "CEEndExternalCaptureRenderCommand";
		};
	};
}
