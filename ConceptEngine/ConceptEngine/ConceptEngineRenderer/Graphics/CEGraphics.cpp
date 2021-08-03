#include "CEGraphics.h"

#include "RenderLayer/CEGraphicsManager.h"
#include "RenderLayer/CommandList.h"
#include "RenderLayer/ICommandContext.h"

CEGraphics::CEGraphics() {
}

bool CEGraphics::Create() {
	if (!CreateManagers()) {
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

bool CEGraphics::CreateManagers() {
	if (!CreateGraphicsManager()) {
		CE_LOG_ERROR("Failed to Instance Graphics Manager")
		CEDebug::DebugBreak();
		return false;
	}

	if (!CreateShaderCompiler()) {
		CE_LOG_ERROR("Failed to Instance Shader Compiler")
		CEDebug::DebugBreak();
		return false;
	}


	if (GraphicsManager->Create()) {
		ICommandContext* commandContext = GraphicsManager->GetDefaultCommandContext();
		GCmdListExecutor.SetContext(commandContext);
	}
	else {
		CE_LOG_ERROR("Failed to Create Graphics Manager")
		CEPlatformMisc::MessageBox("Error", "Failed to initialize Graphics Manager");
		return false;
	}

	if (!CreateTextureManager()) {
		CE_LOG_ERROR("Failed to Instance Texture Manager")
		CEDebug::DebugBreak();
		return false;
	}

	if (!TextureManager->Create()) {
		CE_LOG_ERROR("Failed to Create Texture Manager")
		CEPlatformMisc::MessageBox("Error", "Failed to initialize Texture Manager");
		return false;
	}

	if (!CreateDebugUI()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!DebugUI->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CreateMeshManager()) {
		CE_LOG_ERROR("Failed to Instance Mesh Manager")
		CEDebug::DebugBreak();
		return false;
	}

	if (!MeshManager->Create()) {
		CE_LOG_ERROR("Failed to Create Mesh Manager")
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEGraphics::Release() {
	GraphicsManager->Release();
	TextureManager->Release();
	MeshManager->Release();
	return false;
}

CEDebugUI* CEGraphics::GetDebugUI() {
	return DebugUI;
}
