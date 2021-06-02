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

#include "../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../Core/Application/CECore.h"
#include "../Core/Debug/CEProfiler.h"

#include "../Graphics/Main/Common/CEMaterial.h"

namespace ConceptEngine::Render {

	using namespace Graphics::Main::Rendering;
	using namespace Graphics::Main::RenderLayer;


	static const uint32 ShadowMapSampleCount = 2;

	static std::unordered_map<std::string, Core::Platform::Generic::Debug::CEConsoleVariableEx<bool>> Variables = {
		{"CE.DrawTextureDebugger", Core::Platform::Generic::Debug::CEConsoleVariableEx(false)},
		{"CE.DrawRendererInfo", Core::Platform::Generic::Debug::CEConsoleVariableEx(false)},
		{"CE.EnableSSAO", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.EnableFXAA", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.GFXAADebug", Core::Platform::Generic::Debug::CEConsoleVariableEx(false)},
		{"CE.EnableVariableRateShading", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.PrePassEnabled", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.DrawAABB", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.VSyncEnabled", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.FrustumCullEnabled", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
		{"CE.RayTracingEnabled", Core::Platform::Generic::Debug::CEConsoleVariableEx(true)},
	};

	class CERenderer {
	public:
		virtual bool Create() = 0;
		virtual void Release();
		virtual void Update(const Scene::CEScene& scene) = 0;

		virtual void PerformFrustumCulling(const Scene::CEScene& scene) = 0;
		virtual void PerformFXAA(CECommandList& commandList) = 0;
		virtual void PerformBackBufferBlit(CECommandList& commandList) = 0;
		virtual void PerformAABBDebugPass(CECommandList& commandList) = 0;
		
		virtual void RenderDebugInterface() = 0;

	protected:
		virtual void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event);

		virtual bool CreateBoundingBoxDebugPass() = 0;
		virtual bool CreateAA() = 0;
		virtual bool CreateShadingImage() = 0;

		virtual void ResizeResources(uint32 width, uint32 height) = 0;

		virtual const CEInputLayoutStateCreateInfo& CreateInputLayoutCreateInfo() = 0;

	protected:
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
		Core::Common::CERef<CEComputeShader> ShadingRateShader;

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
