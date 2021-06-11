#include "CECore.h"

#include "../../Graphics/Main/Common/API.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"
#include "../../Core/Platform/CEPlatformActions.h"
#include "../../Graphics/Main/Common/CEPlayground.h"
#include "../../Core/Platform/Generic/Callbacks/CEEngineController.h"
#include "../../Core/Platform/Generic/Debug/CETypedConsole.h"
#include "../../Core/Debug/CEProfiler.h"
#include "../../Core/Debug/CEDebug.h"
#include "../../Core/Threading/CETaskManager.h"
#include "../../Render/CERenderer.h"
#include "../../Graphics/DirectX12/CEDirectX12.h"
#include "../../Graphics/Vulkan/CEVulkan.h"
#include "../../Graphics/OpenGL/CEOpenGL.h"
#include "../../Graphics/Metal/CEMetal.h"
#include "../../Core/Platform/Android/CEAndroid.h"
#include "../../Core/Platform/Mac/CEMac.h"
#include "../../Core/Platform/Windows/CEWindows.h"
#include "../../Core/Platform/iOS/CEiOS.h"
#include "../../Core/Platform/Linux/CELinux.h"
#include "../../Core/Compilers/Cpp/CECppCompiler.h"
#include "../../Core/Compilers/CSharp/CECSCompiler.h"
#include "../../Core/Compilers/Python/CEPyCompiler.h"
#include "../../Core/Compilers/JS/CEJSCompiler.h"
#include "../../Core/Compilers/Schematics/CESchematicsCompiler.h"


CETypedConsole Console;

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Compilers::Language language,
                                                 Generic::Platform::Platform platform) {
	GGraphics = SelectGraphicsAPI(api);
	GCompiler = SelectLanguageCompiler(language);
	GPlatform = SelectPlatform(platform);
}

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Compilers::Language language,
                                                 Generic::Platform::Platform platform,
                                                 Graphics::Main::Common::CEPlayground* playground) :
	CECore(api, language, platform) {
	GPlayground = playground;
}

bool ConceptEngine::Core::Application::CECore::Create() {

	Debug::CEProfiler::Create();

	if (!GPlatform->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Platform");
		return false;
	}

	if (!Threading::CETaskManager::Get().Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!EngineController.Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!GGraphics->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Graphics");
		return false;
	}

	if (GPlayground == nullptr) {
		GPlayground = CreatePlayground();
	}

	Assert(::Common::GPlayground != nullptr);

	GPlatform->SetCallbacks(&EngineController);

	if (!GPlayground->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!Renderer->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to Create Renderer");
		return false;
	}

	GTypedConsole.Create();

	IsRunning = true;

	return true;
}

// CEGraphics* ConceptEngine::Core::Application::CECore::GetGraphics() {
// 	return Graphics;
// }
//
// CECompiler* ConceptEngine::Core::Application::CECore::GetCompiler() {
// 	return Compiler;
// }
//
// ConceptEngine::Core::Generic::Platform::CEPlatform* ConceptEngine::Core::Application::CECore::GetPlatform() {
// 	return Platform;
// }
//
// Common::CEPlayground* ConceptEngine::Core::Application::CECore::GetPlayground() {
// 	return GPlayground;
// }


ConceptEngine::Graphics::Main::CEGraphics*
ConceptEngine::Core::Application::CECore::SelectGraphicsAPI(GraphicsAPI api) {
	SetAPI(api);
	switch (api) {
	case GraphicsAPI::DirectX: {
		return new Graphics::DirectX12::CEDirectX12();
	}
	case GraphicsAPI::Vulkan: {
		return new Graphics::Vulkan::CEVulkan();
	}
	case GraphicsAPI::OpenGL: {
		return new Graphics::OpenGL::CEOpenGL();
	}
	case GraphicsAPI::Metal: {
		return new Graphics::Metal::CEMetal();
	}
	default: {
		return nullptr;
	}
	}
}

ConceptEngine::Core::Generic::Platform::CEPlatform*
ConceptEngine::Core::Application::CECore::SelectPlatform(
	Generic::Platform::Platform platform) {
	switch (platform) {
	case Generic::Platform::Platform::Android:
		return new ConceptEngine::Core::Platform::Android::CEAndroid();
	case Generic::Platform::Platform::iOS:
		return new ConceptEngine::Core::Platform::iOS::CEiOS();
	case Generic::Platform::Platform::Linux:
		return new ConceptEngine::Core::Platform::Linux::CELinux();
	case Generic::Platform::Platform::Mac:
		return new ConceptEngine::Core::Platform::Mac::CEMac();
	case Generic::Platform::Platform::Windows:
		return new ConceptEngine::Core::Platform::Windows::CEWindows();
	case Generic::Platform::Platform::Unknown:
		return nullptr;
	default:
		return nullptr;
	}
}

ConceptEngine::Core::Compilers::CECompiler* ConceptEngine::Core::Application::CECore::SelectLanguageCompiler(
	Compilers::Language language) {
	switch (language) {
	case Compilers::Language::Cpp:
		return new ConceptEngine::Core::Compilers::Cpp::CECppCompiler();
	case Compilers::Language::CSharp:
		return new ConceptEngine::Core::Compilers::CSharp::CECSCompiler();
	case Compilers::Language::JS:
		return new ConceptEngine::Core::Compilers::JS::CEJSCompiler();
	case Compilers::Language::Python:
		return new ConceptEngine::Core::Compilers::Python::CEPyCompiler();
	case Compilers::Language::Schematics:
		return new ConceptEngine::Core::Compilers::Schematics::CESchematicsCompiler();
	case Compilers::Language::Count:
		return nullptr;
	case Compilers::Language::None:
		return nullptr;
	default:
		return nullptr;
	}
}
