#include "CEDirectX12.h"

#include "Managers/CEDXManager.h"
#include "Managers/CEDXRendererManager.h"
#include "Managers/CEDXTextureManager.h"

#include "Rendering/CEDXDebugUI.h"

#include "RenderLayer/CEDXShaderCompiler.h"

#include "../../Core/Debug/CEDebug.h"

using namespace ConceptEngine::Graphics::DirectX12;

CEDirectX12::CEDirectX12(): CEGraphics() {
}

CEDirectX12::~CEDirectX12() {
}

bool CEDirectX12::Create() {
	if (!CEGraphics::Create()) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}
	return true;
}

bool CEDirectX12::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}
	return true;
}

bool CEDirectX12::CreateGraphicsManager() {
	auto manager = new Managers::CEDXManager();
	GraphicsManager = manager;

	return true;
}

bool CEDirectX12::CreateTextureManager() {
	auto manager = new Managers::CEDXTextureManager();
	TextureManager = manager;
	return true;
}

bool CEDirectX12::CreateRendererManager() {
	auto manager = new Managers::CEDXRendererManager();
	RendererManager = manager;
	return true;
}

bool CEDirectX12::CreateDebugUi() {
	auto debugUi = std::make_unique<Rendering::CEDXDebugUI>();
	DebugUI = std::move(debugUi);
	return true;
}

bool CEDirectX12::CreateShaderCompiler() {
	auto shaderCompiler = new RenderLayer::CEDXShaderCompiler();
	if (!shaderCompiler->Create()) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	ShaderCompiler = shaderCompiler;

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
