#include "CEGraphics.h"

#include "RenderLayer/CECommandList.h"

#include "../../Core/Platform/CEPlatformActions.h"

using namespace ConceptEngine::Graphics::Main;

CEGraphics::CEGraphics() {
}

bool CEGraphics::Create() {
	if (!CreateManagers()) {
		return false;
	}
	return true;
}

bool CEGraphics::CreateManagers() {
	if (!CreateGraphicsManager()) {
		return false;
	}

	if (!CreateShaderCompiler()) {
		return false;
	}

	if (!Compiler->Create()) {
		return false;
	}

	if (GraphicsManager->Create()) {
		RenderLayer::CEICommandContext* commandContext = GraphicsManager->GetDefaultCommandContext();
		CommandListExecutor.SetContext(commandContext);
	}
	else {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Graphics Manager");
		return false;
	}

	if (!CreateTextureManager()) {
		return false;
	}

	if (!TextureManager->Create()) {
		return false;
	}

	if (!CreateRendererManager()) {
		return false;
	}

	if (!RendererManager->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Renderer Manager");
		return false;
	}

	if (!CreateDebugUi()) {
		return false;
	}

	if (!DebugUI->Create()) {
		return false;
	}

	return true;
}

//TODO: Change to static Variable for DebugUI and typedef for different rendering API
Rendering::CEDebugUI* CEGraphics::GetDebugUI() {
	return DebugUI.get();
}

ConceptEngine::Core::Common::CEManager* CEGraphics::GetManager(Core::Common::CEManagerType type) const {
	switch (type) {
	case Core::Common::CEManagerType::GraphicsManager:
		return GraphicsManager;
	case Core::Common::CEManagerType::TextureManager:
		return TextureManager;
	case Core::Common::CEManagerType::RendererManager:
		return RendererManager;
	default:
		return nullptr;

	}
}
