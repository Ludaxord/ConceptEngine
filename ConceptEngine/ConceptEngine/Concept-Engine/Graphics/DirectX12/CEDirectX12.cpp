#include "CEDirectX12.h"

#include "Modules/Managers/CEDXManager.h"
#include "Modules/Managers/CEDXRendererManager.h"
#include "Modules/Managers/CEDXTextureManager.h"

using namespace ConceptEngine::Graphics::DirectX12;

CEDirectX12::CEDirectX12(): CEGraphics() {
}

CEDirectX12::~CEDirectX12() {
}

bool CEDirectX12::Create() {
	if (!CEGraphics::Create()) {
		return false;
	}
	return true;
}

bool CEDirectX12::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		return false;
	}
	return true;
}

bool CEDirectX12::CreateGraphicsManager() {
	auto manager = std::make_unique<Modules::Managers::CEDXManager>();
	m_graphicsManager = std::move(manager);

	return true;
}

bool CEDirectX12::CreateTextureManager() {
	auto manager = std::make_unique<Modules::Managers::CEDXTextureManager>();
	m_textureManager = std::move(manager);
	return true;
}

bool CEDirectX12::CreateRendererManager() {
	auto manager = std::make_unique<Modules::Managers::CEDXRendererManager>();
	m_rendererManager = std::move(manager);
	return true;
}

void CEDirectX12::Update() {
}

void CEDirectX12::Render() {
}

void CEDirectX12::Resize() {
}

void CEDirectX12::Destroy() {
}
