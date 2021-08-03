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
	return false;
}

bool CEGraphics::CreateManagers() {
	if (!CreateGraphicsManager()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CreateShaderCompiler()) {
		CEDebug::DebugBreak();
		return false;
	}


	if (GraphicsManager->Create()) {
		ICommandContext* commandContext = GraphicsManager->GetDefaultCommandContext();
		GCmdListExecutor.SetContext(commandContext);
	}
	else {

		CEPlatformMisc::MessageBox("Error", "Failed to initialize Graphics Manager");
		return false;
	}

	if (!CreateTextureManager()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!TextureManager->Create()) {
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
		CEDebug::DebugBreak();
		return false;
	}

	if (!MeshManager->Create()) {
		CE_LOG_ERROR("Failed to create Mesh Manager");
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