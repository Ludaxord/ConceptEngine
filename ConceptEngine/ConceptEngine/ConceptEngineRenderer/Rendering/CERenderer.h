#pragma once
#include "../Time/Timer.h"

#include "../Scene/Actor.h"
#include "../Scene/Scene.h"
#include "../Scene/Camera.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/MeshFactory.h"

#include "DeferredRenderer.h"
#include "ShadowMapRenderer.h"
#include "ScreenSpaceOcclusionRenderer.h"
#include "LightProbeRenderer.h"
#include "SkyboxRenderPass.h"
#include "ForwardRenderer.h"
#include "RayTracer.h"

#include "../RenderLayer/RenderLayer.h"
#include "../RenderLayer/CommandList.h"
#include "../RenderLayer/Viewport.h"

#include "DebugUI.h"

class CERenderer {
public:
	virtual bool Create() = 0;
	virtual void Release() = 0;

	virtual void PerformFrustumCulling(const Scene& Scene) = 0;
	virtual void PerformFXAA(CommandList& InCmdList) = 0;
	virtual void PerformBackBufferBlit(CommandList& InCmdList) = 0;

	virtual void PerformAABBDebugPass(CommandList& InCmdList) = 0;

	virtual void RenderDebugInterface() = 0;

	virtual void Update(const Scene& Scene) = 0;

private:
	void OnWindowResize(const WindowResizeEvent& Event);

	virtual bool CreateBoundingBoxDebugPass() = 0;
	virtual bool CreateAA() = 0;
	virtual bool CreateShadingImage() = 0;

	virtual void ResizeResources(uint32 Width, uint32 Height) = 0;

	CommandList CmdList;

	DeferredRenderer DeferredRenderer;
	ShadowMapRenderer ShadowMapRenderer;
	ScreenSpaceOcclusionRenderer SSAORenderer;
	LightProbeRenderer LightProbeRenderer;
	SkyboxRenderPass SkyboxRenderPass;
	ForwardRenderer ForwardRenderer;
	RayTracer RayTracer;

	FrameResources Resources;
	LightSetup LightSetup;

	CERef<Texture2D> ShadingImage;
	CERef<ComputePipelineState> ShadingRatePipeline;
	CERef<ComputeShader> ShadingRateShader;

	CERef<VertexBuffer> AABBVertexBuffer;
	CERef<IndexBuffer> AABBIndexBuffer;
	CERef<GraphicsPipelineState> AABBDebugPipelineState;
	CERef<VertexShader> AABBVertexShader;
	CERef<PixelShader> AABBPixelShader;

	CERef<GraphicsPipelineState> PostPSO;
	CERef<PixelShader> PostShader;
	CERef<GraphicsPipelineState> FXAAPSO;
	CERef<PixelShader> FXAAShader;
	CERef<GraphicsPipelineState> FXAADebugPSO;
	CERef<PixelShader> FXAADebugShader;

	CERef<GPUProfiler> GPUProfiler;

	uint32 LastFrameNumDrawCalls = 0;
	uint32 LastFrameNumDispatchCalls = 0;
	uint32 LastFrameNumCommands = 0;
};

extern CERenderer* Renderer;
