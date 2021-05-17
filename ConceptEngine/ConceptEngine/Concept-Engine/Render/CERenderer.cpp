#include "CERenderer.h"

#include "../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../Core/Application/CECore.h"

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

	return true;
}

void CERenderer::Release() {
}

void CERenderer::Update(const Scene::CEScene& scene) {
}

void CERenderer::PerformFrustumCulling(const Scene::CEScene& scene) {
}

void CERenderer::PerformFXAA(CECommandList& commandList) {
}

void CERenderer::PerformBackBufferBlit(CECommandList& commandList) {
}

void CERenderer::RenderDebugInterface() {
}

void CERenderer::OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) {
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
}
