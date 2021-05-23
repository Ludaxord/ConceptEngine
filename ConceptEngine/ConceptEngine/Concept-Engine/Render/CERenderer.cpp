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

static const uint32 ShadowMapSampleCount = 2;

static std::unordered_map<std::string, CEConsoleVariableEx<bool>> Variables = {
	{"CE.DrawTextureDebugger", CEConsoleVariableEx(false)},
	{"CE.DrawRendererInfo", CEConsoleVariableEx(false)},
	{"CE.EnableSSAO", CEConsoleVariableEx(true)},
	{"CE.EnableFXAA", CEConsoleVariableEx(true)},
	{"CE.GFXAADebug", CEConsoleVariableEx(false)},
	{"CE.EnableVariableRateShading", CEConsoleVariableEx(true)},
	{"CE.PrePassEnabled", CEConsoleVariableEx(true)},
	{"CE.DrawAABB", CEConsoleVariableEx(true)},
	{"CE.VSyncEnabled", CEConsoleVariableEx(true)},
	{"CE.FrustumCullEnabled", CEConsoleVariableEx(true)},
	{"CE.RayTracingEnalbed", CEConsoleVariableEx(true)},
};

//TODO: right not initialize everything here for now, maybe move it to graphics api renderer in future
bool CERenderer::Create() {
	for (auto variable : Variables) {
		INIT_CONSOLE_VARIABLE(variable.first, &variable.second);
	}

	Resources.MainWindowViewport = dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))
		->CreateViewport(
			EngineController.GetWindow(),
			0,
			0,
			CEFormat::R8G8B8A8_Unorm,
			CEFormat::Unknown
		);

	if (!Resources.MainWindowViewport) {
		return false;
	}

	Resources.CameraBuffer = CreateConstantBuffer(BufferFlag_Default, CEResourceState::Common, nullptr);
	if (!Resources.CameraBuffer) {
		CE_LOG_ERROR("[CERenderer]: Failed to Create Camera Buffer");
		return false;
	}

	Resources.CameraBuffer->SetName("CameraBuffer");

	const auto inputLayout = CreateInputLayoutCreateInfo();

	Resources.StdInputLayout = dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager))->CreateInputLayout(inputLayout);
	if (!Resources.StdInputLayout) {
		return false;
	}

	Resources.StdInputLayout->SetName("Standard InputLayoutState");

	{
		CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = CESamplerMode::Border;
		createInfo.AddressV = CESamplerMode::Border;
		createInfo.AddressW = CESamplerMode::Border;
		createInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		createInfo.ComparisonFunc = CEComparisonFunc::LessEqual;
		createInfo.BorderColor = Math::CEColorF(1.0f, 1.0f, 1.0f, 1.0f);

		Resources.DirectionalShadowSampler = dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))->CreateSamplerState(createInfo);
		if (!Resources.DirectionalShadowSampler) {
			return false;
		}

		Resources.DirectionalShadowSampler->SetName("ShadowMap Sampler");
	}

	{
		CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = CESamplerMode::Wrap;
		createInfo.AddressV = CESamplerMode::Wrap;
		createInfo.AddressW = CESamplerMode::Wrap;
		createInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		createInfo.ComparisonFunc = CEComparisonFunc::LessEqual;

		Resources.PointShadowSampler = dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))->CreateSamplerState(createInfo);
		if (!Resources.PointShadowSampler) {
			return false;
		}

		Resources.PointShadowSampler->SetName("ShadowMap Comparison Sampler");
	}

	GPUProfiler = dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager))->CreateProfiler();
	if (!GPUProfiler) {
		return false;
	}

	Core::Debug::CEProfiler::SetGPUProfiler(GPUProfiler.Get());

	if (!CreateAA()) {
		return false;
	}

	if (!CreateBoundingBoxDebugPass()) {
		return false;
	}

	if (!CreateShadingImage()) {
		return false;
	}

	if (!LightSetup.Create()) {
		return false;
	}

	if (!DeferredRenderer.Create(Resources)) {
		return false;
	}

	if (!ShadowMapRenderer.Create(LightSetup, Resources)) {
		return false;
	}

	if (!SSAORenderer.Create(Resources)) {
		return false;
	}

	if (!LightProbeRenderer.Create(LightSetup, Resources)) {
		return false;
	}

	if (!SkyBoxRenderPass.Create(Resources)) {
		return false;
	}

	if (!ForwardRenderer.Create(Resources)) {
		return false;
	}

	if (IsRayTracingSupported()) {
		if (!RayTracer.Create(Resources)) {
			return false;
		}
	}

	using namespace std::placeholders;
	CommandList.Execute([this] {
		LightProbeRenderer.RenderSkyLightProbe(CommandList, LightSetup, Resources);
	});

	CommandListExecutor.ExecuteCommandList(CommandList);

	EngineController.OnWindowResizedEvent.AddObject(this, &CERenderer::OnWindowResize);

	return true;
}

void CERenderer::Release() {
	CommandListExecutor.WaitForGPU();
	CommandList.Reset();

	DeferredRenderer.Release();
	ShadowMapRenderer.Release();
	SSAORenderer.Release();
	LightProbeRenderer.Release();
	SkyBoxRenderPass.Release();
	ForwardRenderer.Release();
	RayTracer.Release();

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

void CERenderer::Update(const Scene::CEScene& scene) {
	Resources.DeferredVisibleCommands.Clear();
	Resources.ForwardVisibleCommands.Clear();
	Resources.DebugTextures.Clear();

	if (!Variables["CE.FrustumCullEnabled"].GetBool()) {
		for (const CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
			if (command.Material->HasAlphaMask()) {
				Resources.ForwardVisibleCommands.EmplaceBack(command);
			}
			else {
				Resources.DeferredVisibleCommands.EmplaceBack(command);
			}
		}
	}
	else {
		PerformFrustumCulling(scene);
	}
}

void CERenderer::PerformFrustumCulling(const Scene::CEScene& scene) {
}

void CERenderer::PerformFXAA(CECommandList& commandList) {
}

void CERenderer::PerformBackBufferBlit(CECommandList& commandList) {
}

void CERenderer::RenderDebugInterface() {
}

void CERenderer::OnWindowResize(const CEWindowResizeEvent& Event) {
	ResizeResources(Event.Width, Event.Height);
}

bool CERenderer::CreateBoundingBoxDebugPass() {
	return false;
}

bool CERenderer::CreateAA() {
	return false;
}

bool CERenderer::CreateShadingImage() {
	return false;
}

void CERenderer::ResizeResources(uint32 width, uint32 height) {
	if (!Resources.MainWindowViewport->Resize(width, height)) {
		return;
	}

	if (!DeferredRenderer.ResizeResources(Resources)) {
		return;
	}

	if (!SSAORenderer.ResizeResources(Resources)) {
		return;
	}
}

bool CERenderer::IsRayTracingSupported() {
	CERayTracingSupport supportLevel;
	dynamic_cast<CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager))->CheckRayTracingSupport(supportLevel);

	return supportLevel.Tier != CERayTracingTier::NotSupported;
}
