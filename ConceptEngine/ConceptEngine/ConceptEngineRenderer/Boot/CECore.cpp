#include "CECore.h"

#include "Callbacks/CEEngineController.h"
#include "Compiler/CECppCompiler.h"
#include "Compiler/CECSharpCompiler.h"
#include "Compiler/CEGraphsCompiler.h"
#include "Compiler/CEJSCompiler.h"
#include "Compiler/CEPyCompiler.h"
#include "Core/Threading/CETaskManager.h"
#include "Debug/CEProfiler.h"
#include "Graphics/D3D11/CEDirectX11.h"
#include "Graphics/D3D12/CEDirectX12.h"
#include "Graphics/Metal/CEMetal.h"
#include "Graphics/OpenGL/CEOpenGL.h"
#include "Graphics/Vulkan/CEVulkan.h"
#include "Graphics/WebGL/CEWebGL.h"
#include "Platform/CEPlatform.h"
#include "Platform/Android/CEAndroid.h"
#include "Platform/iOS/CEiOS.h"
#include "Platform/Linux/CELinux.h"
#include "Platform/Mac/CEMac.h"
#include "Platform/Windows/CEWindows.h"
#include "Project/CEPlayground.h"

CETypedConsole Console;

CECore::CECore(CEEngineConfig& EConfig) : CECore(EConfig, nullptr) {
}

CECore::CECore(CEEngineConfig& EConfig, CEPlayground* InPlayground) {
	GGraphics = SetGraphicsAPI(EConfig.GraphicsAPI);
	GCompiler = SetLanguageCompiler(EConfig.ScriptingLanguage);
	GPlatform = SetPlatform(EConfig.PlatformBoot);
	GPlayground = InPlayground;
}

bool CECore::Create() {
	CEProfiler::Create();

	if (!GPlatform->Create()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Create Platform");
		return false;
	}

	if (!CETaskManager::Get().Create()) {
		CEDebug::DebugBreak();
	}

	if (!GEngineController.Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!GGraphics->Create()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Create Graphics");
	}

	if (GPlayground == nullptr) {
		GPlayground = CreatePlayground();
	}

	Assert(GPlayground != nullptr);

	GPlatform->SetCallbacks(&GEngineController);

	if (!GPlayground->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!Renderer->Create()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Create Graphics");
		return false;
	}

	Console.Create();

	bIsRunning = true;

	return true;
}

CEGraphics* CECore::GetGraphics() {
	return GGraphics;
}

CECompiler* CECore::GetCompiler() {
	return GCompiler;
}

CEPlatform* CECore::GetPlatform() {
	return GPlatform;
}

CEPlayground* CECore::GetPlayground() {
	return GPlayground;
}

CEGraphics* CECore::SetGraphicsAPI(GraphicsAPI GApi) {
	switch (GApi) {
	case GraphicsAPI::DirectX11:
		return new CEDirectX11();
	case GraphicsAPI::DirectX12:
		return new CEDirectX12();
	case GraphicsAPI::Metal:
		return new CEMetal();
	case GraphicsAPI::OpenGL:
		return new CEOpenGL();
	case GraphicsAPI::Vulkan:
		return new CEVulkan();
	case GraphicsAPI::WebGL:
		return new CEWebGL();
	default:
		return nullptr;
	}
}

CECompiler* CECore::SetLanguageCompiler(ScriptingLanguage SLanguage) {
	switch (SLanguage) {
	case ScriptingLanguage::CSharp:
		return new CECSharpCompiler();
	case ScriptingLanguage::Cpp:
		return new CECppCompiler();
	case ScriptingLanguage::Graphs:
		return new CEGraphsCompiler();
	case ScriptingLanguage::JavaScript:
		return new CEJSCompiler();
	case ScriptingLanguage::Python:
		return new CEPyCompiler();
	case ScriptingLanguage::None:
	default:
		return nullptr;


	}
}

CEPlatform* CECore::SetPlatform(PlatformBoot PBoot) {
	switch (PBoot) {
	case PlatformBoot::Windows: return new CEWindows();
	case PlatformBoot::Mac: return new CEMac();
	case PlatformBoot::Linux: return new CELinux();
	case PlatformBoot::Android: return new CEAndroid();
	case PlatformBoot::iOS: return new CEiOS();
	default: return nullptr;
	}
}
