#include "CEDirectX12.h"

#include "Modules/Managers/CEDXManager.h"

using namespace ConceptEngine::Graphics::DirectX12;

CEDirectX12::CEDirectX12(): CEGraphics() {
}

CEDirectX12::~CEDirectX12() {
}

bool CEDirectX12::Create() {

	return true;
}

bool CEDirectX12::CreateManagers() {
	if (!CreateGraphicsManager()) {
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
