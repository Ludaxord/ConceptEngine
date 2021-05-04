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

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateGraphicsManager() {
	return true;
}

bool ConceptEngine::Graphics::OpenGL::CEOpenGL::CreateTextureManager() {
	return true;
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Update() {
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Render() {
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Resize() {
}

void ConceptEngine::Graphics::OpenGL::CEOpenGL::Destroy() {
}
