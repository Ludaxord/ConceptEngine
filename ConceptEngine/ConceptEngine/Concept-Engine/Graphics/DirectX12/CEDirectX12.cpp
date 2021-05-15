#include "CEDirectX12.h"

#include "Modules/Managers/CEDXManager.h"
#include "Modules/Managers/CEDXRendererManager.h"
#include "Modules/Managers/CEDXTextureManager.h"

#include "Modules/Rendering/CEDXDebugUI.h"

#include "Modules/RenderLayer/CEDXShaderCompiler.h"

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
	GraphicsManager = std::move(manager);

	return true;
}

bool CEDirectX12::CreateTextureManager() {
	auto manager = std::make_unique<Modules::Managers::CEDXTextureManager>();
	TextureManager = std::move(manager);
	return true;
}

bool CEDirectX12::CreateRendererManager() {
	auto manager = std::make_unique<Modules::Managers::CEDXRendererManager>();
	RendererManager = std::move(manager);
	return true;
}

bool CEDirectX12::CreateDebugUi() {
	auto debugUi = std::make_unique<Modules::Rendering::CEDXDebugUI>();
	DebugUI = std::move(debugUi);
	return true;
}

bool CEDirectX12::CreateShaderCompiler() {
	Compiler = new Modules::RenderLayer::CEDXShaderCompiler();
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
