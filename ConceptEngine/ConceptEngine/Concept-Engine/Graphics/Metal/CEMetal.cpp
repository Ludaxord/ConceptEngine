#include "CEMetal.h"

ConceptEngine::Graphics::Metal::CEMetal::CEMetal() : CEGraphics() {
}

ConceptEngine::Graphics::Metal::CEMetal::~CEMetal() {
}

bool ConceptEngine::Graphics::Metal::CEMetal::Create() {
	if (!CEGraphics::Create()) {
		return false;
	}
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		return false;
	}
	return true;
}

void ConceptEngine::Graphics::Metal::CEMetal::Update(Time::CETimestamp DeltaTime,
	boost::function<void()> ExecuteFunction) {
	ExecuteFunction();
}

void ConceptEngine::Graphics::Metal::CEMetal::Render() {
}

void ConceptEngine::Graphics::Metal::CEMetal::Resize() {
}

void ConceptEngine::Graphics::Metal::CEMetal::Destroy() {
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateRendererManager() {
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateGraphicsManager() {
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateTextureManager() {
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateDebugUi() {
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateShaderCompiler() {
	return true;
}

bool ConceptEngine::Graphics::Metal::CEMetal::CreateMeshManager() {
	return true;
}
