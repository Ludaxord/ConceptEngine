#include "CEDirectX12.h"

#include "D3D12/CEDX12DebugUI.h"
#include "D3D12/CEDX12ShaderCompiler.h"
#include "Managers/CEDX12Manager.h"
#include "Managers/CEDX12MeshManager.h"
#include "Managers/CEDX12TextureManager.h"
#include "Project/CEPlayground.h"
#include "Rendering/CERenderer.h"

CEDirectX12::CEDirectX12() : CEGraphics() {
}

CEDirectX12::~CEDirectX12() {
}

bool CEDirectX12::Create() {
	if (!CEGraphics::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEDirectX12::CreateManagers() {
	if (!CEGraphics::CreateManagers()) {
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

bool CEDirectX12::Release() {
	return true;
}

void CEDirectX12::Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) {
	GPlayground->Update(DeltaTime);
	ExecuteFunction();
	Renderer->Update(*GPlayground->Scene);
}

bool CEDirectX12::Resize() {
	return false;
}

bool CEDirectX12::CreateGraphicsManager() {
	GraphicsManager = new CEDX12Manager();
	return true;
}

bool CEDirectX12::CreateTextureManager() {
	TextureManager = new CEDX12TextureManager();
	return true;
}

bool CEDirectX12::CreateMeshManager() {
	MeshManager = new CEDX12MeshManager();
	return true;
}

bool CEDirectX12::CreateShaderCompiler() {
	if (auto shaderCompiler = new CEDX12ShaderCompiler(); !shaderCompiler->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEDirectX12::CreateDebugUI() {
	DebugUI = new CEDX12DebugUI();
	return true;
}
