#include "CERenderer.h"

#include "../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../Core/Application/CECore.h"
#include "../Core/Debug/CEProfiler.h"

#include "../Graphics/Main/Common/CEMaterial.h"

#include <boost/bind.hpp>

using namespace ConceptEngine::Render;
using namespace ConceptEngine::Core::Platform::Generic::Debug;
using namespace ConceptEngine::Core::Generic::Platform;
using namespace ConceptEngine::Graphics::Main::Managers;
using namespace ConceptEngine::Core::Common;

void CERenderer::Release() {
	CE_LOG_WARNING("[CERenderer] Releasing...");
	
	CommandListExecutor.WaitForGPU();
	CommandList.Reset();

	DeferredRenderer->Release();
	
	ShadowMapRenderer->Release();
	SSAORenderer->Release();
	LightProbeRenderer->Release();
	SkyBoxRenderPass->Release();
	ForwardRenderer->Release();
	RayTracer->Release();

	Resources.Release();
	LightSetup.Release();

	AABBVertexBuffer.Reset();
	AABBIndexBuffer.Reset();
	AABBDebugPipelineState.Reset();
	AABBVertexShader.Reset();
	AABBPixelShader.Reset();

	PostPipelineState.Reset();
	PostShader.Reset();
	FXAAPipelineState.Reset();
	FXAAShader.Reset();
	FXAADebugPipelineState.Reset();
	FXAADebugShader.Reset();

	ShadingImage.Reset();
	ShadingRatePipeline.Reset();
	ShadingRateShader.Reset();

	GPUProfiler.Reset();
	Core::Debug::CEProfiler::SetGPUProfiler(nullptr);

	LastFrameNumDrawCalls = 0;
	LastFrameNumDispatchCalls = 0;
	LastFrameNumCommands = 0;
}

void CERenderer::OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) {
	ResizeResources(Event.Width, Event.Height);
}
