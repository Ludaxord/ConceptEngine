#include "CEVulkan.h"

ConceptEngine::Graphics::Vulkan::CEVulkan::CEVulkan(): CEGraphics() {
}

ConceptEngine::Graphics::Vulkan::CEVulkan::~CEVulkan() {
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::Create() {
	if (!CEGraphics::Create()) {
		return false;
	}
	return true;
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		return false;
	}
	return true;
}

void ConceptEngine::Graphics::Vulkan::CEVulkan::Update(Time::CETimestamp DeltaTime) {
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

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateRendererManager() {
	return true;
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateDebugUi() {
	return true;
}

bool ConceptEngine::Graphics::Vulkan::CEVulkan::CreateShaderCompiler() {
	return true;
}
