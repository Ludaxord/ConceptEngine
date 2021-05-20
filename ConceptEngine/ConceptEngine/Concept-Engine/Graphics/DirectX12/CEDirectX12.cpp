#include "CEDirectX12.h"

#include "Managers/CEDXManager.h"
#include "Managers/CEDXRendererManager.h"
#include "Managers/CEDXTextureManager.h"

#include "Rendering/CEDXDebugUI.h"

#include "RenderLayer/CEDXShaderCompiler.h"

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
	auto manager = std::make_unique<Managers::CEDXManager>();
	GraphicsManager = std::move(manager);

	return true;
}

bool CEDirectX12::CreateTextureManager() {
	auto manager = std::make_unique<Managers::CEDXTextureManager>();
	TextureManager = std::move(manager);
	return true;
}

bool CEDirectX12::CreateRendererManager() {
	auto manager = std::make_unique<Managers::CEDXRendererManager>();
	RendererManager = std::move(manager);
	return true;
}

bool CEDirectX12::CreateDebugUi() {
	auto debugUi = std::make_unique<Rendering::CEDXDebugUI>();
	DebugUI = std::move(debugUi);
	return true;
}

bool CEDirectX12::CreateShaderCompiler() {
	Compiler = new RenderLayer::CEDXShaderCompiler();
	return true;
}

void CEDirectX12::Update(Time::CETimestamp DeltaTime) {
}

void CEDirectX12::Render() {
}

void CEDirectX12::Resize() {
}

void CEDirectX12::Destroy() {
}
