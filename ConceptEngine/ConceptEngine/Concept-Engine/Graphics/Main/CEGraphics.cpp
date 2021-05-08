#include "CEGraphics.h"

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

	if (!m_graphicsManager->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Graphics Manager");
		return false;
	}

	if (!CreateTextureManager()) {
		return false;
	}

	if (!m_textureManager->Create()) {
		return false;
	}

	if (!CreateRendererManager()) {
		return false;
	}

	if (!m_rendererManager->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Renderer Manager");
		return false;
	}

	if (!CreateDebugUi()) {
		return false;
	}

	if (!m_debugUi->Create()) {
		return false;
	}

	return true;
}
