#pragma once
#include "Scene/CEScene.h"
#include "../Core/Common/CEEvents.h"
#include "../Graphics/Main/RenderLayer/CECommandList.h"
#include "../Graphics/Main/Rendering/CEDeferredRenderer.h"
#include "../Graphics/Main/Rendering/CERayTracer.h"
#include "../Graphics/Main/Rendering/CEShadowMapRenderer.h"
#include "../Graphics/Main/Rendering/CEScreenSpaceOcclusionRenderer.h"
#include "../Graphics/Main/Rendering/CELightProbeRenderer.h"
#include "../Graphics/Main/Rendering/CESkyBoxRenderPass.h"
#include "../Graphics/Main/Rendering/CEForwardRenderer.h"
#include "../Graphics/Main/Rendering/CEFrameResources.h"
#include "../Graphics/Main/Rendering/CELightSetup.h"
#include "../Graphics/Main/RenderLayer/CETexture.h"
#include "../Graphics/Main/RenderLayer/CEPipelineState.h"
#include "../Graphics/Main/RenderLayer/CEPipelineState.h"
#include "../Graphics/Main/RenderLayer/CEShader.h"
#include "../Graphics/Main/RenderLayer/CEBuffer.h"
#include "../Graphics/Main/RenderLayer/CEGPUProfiler.h"


namespace ConceptEngine::Render {

	using namespace Graphics::Main::Rendering;
	using namespace Graphics::Main::RenderLayer;

	class CERenderer {
	public:
		virtual bool Create();
		virtual void Release();
		virtual void Update(const Scene::CEScene& scene);

		virtual void PerformFrustumCulling(const Scene::CEScene& scene);
		virtual void PerformFXAA(CECommandList& commandList);
		virtual void PerformBackBufferBlit(CECommandList& commandList);

		virtual void RenderDebugInterface();

	private:
		virtual void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event);

		virtual bool CreateBoundingBoxDebugPass();
		virtual bool CreateAA();
		virtual bool CreateShadingImage();

		virtual void ResizeResources(uint32 width, uint32 height);

		virtual Core::Common::CERef<CEConstantBuffer> CreateConstantBuffer(uint32 Flags, CEResourceState InitialState,
		                                                                   const CEResourceData* InitialData) = 0;

		virtual const CEInputLayoutStateCreateInfo& CreateInputLayoutCreateInfo() = 0;
		
		virtual bool IsRayTracingSupported();

	private:
		CECommandList CommandList;

		//TODO: Move to some kind of container because object types are same in some situations;

		CEDeferredRenderer DeferredRenderer;
		CEShadowMapRenderer ShadowMapRenderer;
		CEScreenSpaceOcclusionRenderer SSAORenderer;
		CELightProbeRenderer LightProbeRenderer;
		CESkyBoxRenderPass SkyBoxRenderPass;
		CEForwardRenderer ForwardRenderer;
		CERayTracer RayTracer;

		CEFrameResources Resources;
		CELightSetup LightSetup;

		Core::Common::CERef<CETexture2D> ShadingImage;
		Core::Common::CERef<CEComputePipelineState> ShadingRatePipeline;
		Core::Common::CERef<CEComputeShader> CEShadingRateShader;

		Core::Common::CERef<Graphics::Main::RenderLayer::CEVertexBuffer> AABBVertexBuffer;
		Core::Common::CERef<Graphics::Main::RenderLayer::CEIndexBuffer> AABBIndexBuffer;
		Core::Common::CERef<CEGraphicsPipelineState> AABBDebugPipelineState;
		Core::Common::CERef<CEVertexShader> AABBVertexShader;
		Core::Common::CERef<CEPixelShader> AABBPixelShader;

		Core::Common::CERef<CEGraphicsPipelineState> PostPipelineState;
		Core::Common::CERef<CEPixelShader> PostShader;
		Core::Common::CERef<CEGraphicsPipelineState> FXAAPipelineState;
		Core::Common::CERef<CEPixelShader> FXAAShader;
		Core::Common::CERef<CEGraphicsPipelineState> FXAADebugPipelineState;
		Core::Common::CERef<CEPixelShader> FXAADebugShader;

		Core::Common::CERef<CEGPUProfiler> GPUProfiler;

		uint32 LastFrameNumDrawCalls = 0;
		uint32 LastFrameNumDispatchCalls = 0;
		uint32 LastFrameNumCommands = 0;
	};
}

extern ConceptEngine::Render::CERenderer* Renderer;
