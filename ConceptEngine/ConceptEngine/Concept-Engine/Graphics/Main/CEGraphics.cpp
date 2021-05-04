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

	if (!CreateTextureManager()) {
		return false;
	}

	if (!CreateRendererManager()) {
		return false;
	}
	return true;
}
