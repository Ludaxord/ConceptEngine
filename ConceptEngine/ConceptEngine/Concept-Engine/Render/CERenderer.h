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
	class CERenderer {
	public:
		bool Create();
		void Release();
		void Update(const Scene::CEScene& scene);

		void PerformFrustumCulling(const Scene::CEScene& scene);
		void PerformFXAA(Graphics::Main::RenderLayer::CECommandList& commandList);
		void PerformBackBufferBlit(Graphics::Main::RenderLayer::CECommandList& commandList);

		void RenderDebugInterface();

	private:
		void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event);

		bool CreateBoundingBoxDebugPass();
		bool CreateAA();
		bool CreateShadingImage();

		void ResizeResources(uint32 width, uint32 height);

	private:
		Graphics::Main::RenderLayer::CECommandList CommandList;

		//TODO: Move to some kind of container because object types are same in some situations;

		Graphics::Main::Rendering::CEDeferredRenderer DeferredRenderer;
		Graphics::Main::Rendering::CEShadowMapRenderer ShadowMapRenderer;
		Graphics::Main::Rendering::CEScreenSpaceOcclusionRenderer SSAORenderer;
		Graphics::Main::Rendering::CELightProbeRenderer LightProbeRenderer;
		Graphics::Main::Rendering::CESkyBoxRenderPass SkyBoxRenderPass;
		Graphics::Main::Rendering::CEForwardRenderer ForwardRenderer;
		Graphics::Main::Rendering::CERayTracer RayTracer;

		Graphics::Main::Rendering::CEFrameResources Resources;
		Graphics::Main::Rendering::CELightSetup LightSetup;

		Core::Common::CERef<Graphics::Main::RenderLayer::CETexture2D> ShadingImage;
		Core::Common::CERef<CEComputePipelineState> ShadingRatePipeline;
		Core::Common::CERef<CEComputeShader> CEShadingRateShader;

		Core::Common::CERef<CEVertexBuffer> AABBVertexBuffer;
		Core::Common::CERef<CEIndexBuffer> AABBIndexBuffer;
		Core::Common::CERef<CEGraphicsPipelineState> AABBDebugPipelineState;
		Core::Common::CERef<CEVertexShader> AABBVertexShader;
		Core::Common::CERef<CEIndexShader> AABBIndexShader;

		Core::Common::CERef<CEGraphicsPipelineState> PostPipelineState;
		Core::Common::CERef<CEPixelShader> PostShader;
		Core::Common::CERef<CEGraphicsPipelineState> FXAAPipelineState;
		Core::Common::CERef<CEPixelShader> FXAAShader;
		Core::Common::CERef<CEGraphicsPipelineState> FXAADebugPipelineState;
		Core::Common::CERef<CEPixelShader> FXAADebugShader;

		Core::Common::CERef<Graphics::Main::RenderLayer::CEGPUProfiler> GPUProfiler;

		uint32 LastFrameNumDrawCalls = 0;
		uint32 LastFrameNumDispatchCalls = 0;
		uint32 LastFrameNumCommands = 0;
	};
}

extern ConceptEngine::Render::CERenderer Renderer;
