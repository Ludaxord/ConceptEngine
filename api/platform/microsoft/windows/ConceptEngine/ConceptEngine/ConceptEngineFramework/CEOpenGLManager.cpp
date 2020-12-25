#include "CEOpenGLManager.h"

#include "CEOpenGLDevice.h"

CEOpenGLManager::CEOpenGLManager(const std::wstring& name, int width, int height, bool vSync): CEManager(
	name, width, height, vSync) {
}

bool CEOpenGLManager::LoadContent() {
	return false;
}

void CEOpenGLManager::UnloadContent() {
}

void CEOpenGLManager::OnUpdate() {
}

void CEOpenGLManager::OnRender() {
}

void CEOpenGLManager::OnKeyPressed() {
}

void CEOpenGLManager::OnMouseWheel() {
}

void CEOpenGLManager::OnResize() {
}

bool CEOpenGLManager::Initialize() {
	return false;
}

void CEOpenGLManager::Destroy() {
}

void CEOpenGLManager::OnKeyReleased() {
}

void CEOpenGLManager::OnMouseMoved() {
}

void CEOpenGLManager::OnMouseButtonPressed() {
}

void CEOpenGLManager::OnMouseButtonReleased() {
}

void CEOpenGLManager::OnWindowDestroy() {
}

CEDevice CEOpenGLManager::GetDevice() {
	return CEOpenGLDevice();
}
