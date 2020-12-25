#include "CEMetalManager.h"

#include "CEMetalDevice.h"

CEMetalManager::CEMetalManager(const std::wstring& name, int width, int height, bool vSync): super(
	name, width, height, vSync) {
}

bool CEMetalManager::LoadContent() {
	return false;
}

void CEMetalManager::UnloadContent() {
}

void CEMetalManager::OnUpdate() {
}

void CEMetalManager::OnRender() {
}

void CEMetalManager::OnKeyPressed() {
}

void CEMetalManager::OnMouseWheel() {
}

void CEMetalManager::OnResize() {
}

bool CEMetalManager::Initialize() {
	return false;
}

void CEMetalManager::Destroy() {
}

void CEMetalManager::OnKeyReleased() {
}

void CEMetalManager::OnMouseMoved() {
}

void CEMetalManager::OnMouseButtonPressed() {
}

void CEMetalManager::OnMouseButtonReleased() {
}

void CEMetalManager::OnWindowDestroy() {
}

CEDevice CEMetalManager::GetDevice() {
	return CEMetalDevice();
}
