#include "CEVulkan.h"

ConceptEngine::Graphics::Vulkan::CEVulkan::CEVulkan(): CEGraphics() {
}

ConceptEngine::Graphics::Vulkan::CEVulkan::~CEVulkan() {
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::Create() {

	return true;
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateManagers() {
	if (!CreateGraphicsManager()) {
		return false;
	}
	return true;
}

void ConceptEngine::Graphics::Vulkan::CEVulkan::Update() {
}

void ConceptEngine::Graphics::Vulkan::CEVulkan::Render() {
}

void ConceptEngine::Graphics::Vulkan::CEVulkan::Resize() {
}

void ConceptEngine::Graphics::Vulkan::CEVulkan::Destroy() {
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateGraphicsManager() {
	return true;
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateTextureManager() {
	return true;
}
