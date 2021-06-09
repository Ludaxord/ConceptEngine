#include "CECore.h"

using namespace ConceptEngine::Core::Application;
using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Core::Compilers;
using namespace ConceptEngine::Core::Platform;
using namespace ConceptEngine::Core::Generic::Platform;

CETypedConsole Console;

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Language language,
                                                 Generic::Platform::Platform platform) {
	Graphics = SelectGraphicsAPI(api);
	Compiler = SelectLanguageCompiler(language);
	Platform = SelectPlatform(platform);
}

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Compilers::Language language,
                                                 Generic::Platform::Platform platform,
                                                 Graphics::Main::Common::CEPlayground* playground) :
	CECore(api, language, platform) {
	GPlayground = playground;
}

bool ConceptEngine::Core::Application::CECore::Create() {

	Debug::CEProfiler::Create();

	if (!Platform->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Platform");
		return false;
	}

	if (!Threading::CETaskManager::Get().Create()) {
		Debug::CEDebug::DebugBreak();
		return false;
	}

	if (!EngineController.Create()) {
		Debug::CEDebug::DebugBreak();
		return false;
	}

	if (!Graphics->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to initialize Graphics");
		return false;
	}

	if (GPlayground == nullptr) {
		GPlayground = CreatePlayground();
	}

	Assert(::Common::GPlayground != nullptr);

	Platform->SetCallbacks(&EngineController);

	if (!GPlayground->Create()) {
		Debug::CEDebug::DebugBreak();
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

CEGraphics* ConceptEngine::Core::Application::CECore::GetGraphics() {
	return Graphics;
}

CECompiler* ConceptEngine::Core::Application::CECore::GetCompiler() {
	return Compiler;
}

ConceptEngine::Core::Generic::Platform::CEPlatform* ConceptEngine::Core::Application::CECore::GetPlatform() {
	return Platform;
}

Common::CEPlayground* ConceptEngine::Core::Application::CECore::GetPlayground() {
	return GPlayground;
}


CEGraphics* ConceptEngine::Core::Application::CECore::SelectGraphicsAPI(GraphicsAPI api) {
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

ConceptEngine::Core::Generic::Platform::CEPlatform* ConceptEngine::Core::Application::CECore::SelectPlatform(
	Generic::Platform::Platform platform) {
	switch (platform) {
	case Generic::Platform::Platform::Android:
		return new Android::CEAndroid();
	case Generic::Platform::Platform::iOS:
		return new iOS::CEiOS();
	case Generic::Platform::Platform::Linux:
		return new Linux::CELinux();
	case Generic::Platform::Platform::Mac:
		return new Mac::CEMac();
	case Generic::Platform::Platform::Windows:
		return new Platform::Windows::CEWindows();
	case Generic::Platform::Platform::Unknown:
		return nullptr;
	default:
		return nullptr;
	}
}

CECompiler* ConceptEngine::Core::Application::CECore::SelectLanguageCompiler(Language language) {
	switch (language) {
	case Language::Cpp:
		return new Cpp::CECppCompiler();
	case Language::CSharp:
		return new CSharp::CECSCompiler();
	case Language::JS:
		return new JS::CEJSCompiler();
	case Language::Python:
		return new Python::CEPyCompiler();
	case Language::Schematics:
		return new Schematics::CESchematicsCompiler();
	case Language::Count:
		return nullptr;
	case Language::None:
		return nullptr;
	default:
		return nullptr;
	}
}
