#include "CEDXRenderer.h"

#include "../../../Core/Application/CECore.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Render;
using namespace ConceptEngine::Render::Scene;
using namespace ConceptEngine::Graphics::Main::RenderLayer;
using namespace ConceptEngine::Core::Common;

ConceptEngine::Render::CERenderer* Renderer = new CEDXRenderer();

struct CEDXCameraBufferDesc {

};

bool CEDXRenderer::Create() {

	for (auto variable : ConceptEngine::Render::Variables) {
		INIT_CONSOLE_VARIABLE(variable.first, &variable.second);
	}

	Resources.MainWindowViewport = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))
		->CreateViewport(
			Core::Platform::Generic::Callbacks::EngineController.GetWindow(),
			0,
			0,
			CEFormat::R8G8B8A8_Unorm,
			CEFormat::Unknown
		);

	if (!Resources.MainWindowViewport) {
		return false;
	}

	Resources.CameraBuffer = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateConstantBuffer<CEDXCameraBufferDesc>(
		BufferFlag_Default, CEResourceState::Common, nullptr
	);

	if (!Resources.CameraBuffer) {
		CE_LOG_ERROR("[CERenderer]: Failed to Create Camera Buffer");
		return false;
	}

	Resources.CameraBuffer->SetName("CameraBuffer");

	const auto inputLayout = CreateInputLayoutCreateInfo();

	Resources.StdInputLayout = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
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

		Resources.DirectionalShadowSampler = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
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

		Resources.PointShadowSampler = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))->CreateSamplerState(createInfo);
		if (!Resources.PointShadowSampler) {
			return false;
		}

		Resources.PointShadowSampler->SetName("ShadowMap Comparison Sampler");
	}

	GPUProfiler = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
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

	if (dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->IsRayTracingSupported()) {
		if (!RayTracer.Create(Resources)) {
			return false;
		}
	}

	using namespace std::placeholders;
	CommandList.Execute([this] {
		LightProbeRenderer.RenderSkyLightProbe(CommandList, LightSetup, Resources);
	});

	CommandListExecutor.ExecuteCommandList(CommandList);

	Core::Platform::Generic::Callbacks::EngineController.OnWindowResizedEvent.AddObject(
		this, &CEDXRenderer::OnWindowResize);

	return true;
}

void CEDXRenderer::Release() {
	CERenderer::Release();
}

void CEDXRenderer::PerformFrustumCulling(const CEScene& scene) {
	CERenderer::PerformFrustumCulling(scene);
}

void CEDXRenderer::PerformFXAA(CECommandList& commandList) {
	CERenderer::PerformFXAA(commandList);
}

void CEDXRenderer::PerformBackBufferBlit(CECommandList& commandList) {
	CERenderer::PerformBackBufferBlit(commandList);
}

void CEDXRenderer::RenderDebugInterface() {
	CERenderer::RenderDebugInterface();
}

void CEDXRenderer::OnWindowResize(const CEWindowResizeEvent& Event) {
}

bool CEDXRenderer::CreateBoundingBoxDebugPass() {
	return false;
}

bool CEDXRenderer::CreateAA() {
	return false;
}

bool CEDXRenderer::CreateShadingImage() {
	return false;
}

const CEInputLayoutStateCreateInfo& CEDXRenderer::CreateInputLayoutCreateInfo() {
	const CEInputLayoutStateCreateInfo InputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
		{"NORMAL", 0, CEFormat::R32G32B32_Float, 0, 12, CEInputClassification::Vertex, 0},
		{"TANGENT", 0, CEFormat::R32G32B32_Float, 0, 24, CEInputClassification::Vertex, 0},
		{"TEXCOORD", 0, CEFormat::R32G32_Float, 0, 36, CEInputClassification::Vertex, 0}
	};

	return InputLayout;
}


void CEDXRenderer::Update(const CEScene& scene) {
	Resources.DeferredVisibleCommands.Clear();
	Resources.ForwardVisibleCommands.Clear();
	Resources.DebugTextures.Clear();

	if (!ConceptEngine::Render::Variables["CE.FrustumCullEnabled"].GetBool()) {
		for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
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


void CEDXRenderer::ResizeResources(uint32 width, uint32 height) {
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
