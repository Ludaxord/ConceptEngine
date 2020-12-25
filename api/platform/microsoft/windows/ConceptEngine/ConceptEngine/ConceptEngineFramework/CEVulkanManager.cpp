#include "CEVulkanManager.h"

#include "CEVulkanDevice.h"

CEVulkanManager::CEVulkanManager(const std::wstring& name, int width, int height, bool vSync): CEManager(
	name, width, height, vSync) {
}

bool CEVulkanManager::LoadContent() {
	return false;
}

void CEVulkanManager::UnloadContent() {
}

void CEVulkanManager::OnUpdate() {
}

void CEVulkanManager::OnRender() {
}

void CEVulkanManager::OnKeyPressed() {
}

void CEVulkanManager::OnMouseWheel() {
}

void CEVulkanManager::OnResize() {
}

bool CEVulkanManager::Initialize() {
	return false;
}

void CEVulkanManager::Destroy() {
}

void CEVulkanManager::OnKeyReleased() {
}

void CEVulkanManager::OnMouseMoved() {
}

void CEVulkanManager::OnMouseButtonPressed() {
}

void CEVulkanManager::OnMouseButtonReleased() {
}

void CEVulkanManager::OnWindowDestroy() {
}

CEDevice CEVulkanManager::GetDevice() {
	return CEVulkanDevice();
}
