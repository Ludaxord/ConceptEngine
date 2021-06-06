#include "CEOpenGL.h"

ConceptEngine::Graphics::OpenGL::CEOpenGL::CEOpenGL() : CEGraphics() {
}

ConceptEngine::Graphics::OpenGL::CEOpenGL::~CEOpenGL() {
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::Create() {
	if (!CEGraphics::Create()) {
		return false;
	}
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		return false;
	}
	return true;
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Update(Time::CETimestamp DeltaTime,
	boost::function<void()> ExecuteFunction) {
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateGraphicsManager() {
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateTextureManager() {
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateDebugUi() {
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateShaderCompiler() {
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateMeshManager() {
	return true;
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Render() {
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Resize() {
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Destroy() {
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateRendererManager() {
	return true;
}
