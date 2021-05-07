#include "CECore.h"

#include "../../Graphics/DirectX12/CEDirectX12.h"
#include "../../Graphics/Vulkan/CEVulkan.h"
#include "../../Graphics/OpenGL/CEOpenGL.h"
#include "../../Graphics/Metal/CEMetal.h"

#include "../Compilers/Cpp/CECppCompiler.h"
#include "../Compilers/CSharp/CECSCompiler.h"
#include "../Compilers/JS/CEJSCompiler.h"
#include "../Compilers/Python/CEPyCompiler.h"
#include "../Compilers/Schematics/CESchematicsCompiler.h"

#include "../Platform/Windows/CEWindows.h"
#include "../Platform/Mac/CEMac.h"
#include "../Platform/iOS/CEiOS.h"
#include "../Platform/Android/CEAndroid.h"
#include "../Platform/Linux/CELinux.h"

#include "../Debug/CEProfiler.h"
#include "../Threading/CETaskManager.h"

#include "../Platform/Generic/Callbacks/CEEngineController.h"

using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Core::Compilers;
using namespace ConceptEngine::Core::Platform;

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
	::Common::GPlayground = playground;
}

bool ConceptEngine::Core::Application::CECore::Create() {
	Debug::CEProfiler::Init();

	if (Platform->Create()) {
		return false;
	}

	if (!Threading::CETaskManager::Get().Create()) {
		return false;
	}

	if (!Platform::Generic::Callbacks::EngineController.Create()) {
		return false;
	}

	if (!Graphics->Create()) {
		return false;
	}

	if (::Common::GPlayground == nullptr) {
		::Common::GPlayground = ::Common::CreatePlayground();
	}

	if (!::Common::GPlayground->Create()) {
		return false;
	}

	Platform->SetCallbacks(&Platform::Generic::Callbacks::EngineController);

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
	return ::Common::GPlayground;
}


CEGraphics* ConceptEngine::Core::Application::CECore::SelectGraphicsAPI(GraphicsAPI api) {
	switch (api) {
	case GraphicsAPI::DirectX:
		return new Graphics::DirectX12::CEDirectX12();
	case GraphicsAPI::Vulkan:
		return new Graphics::Vulkan::CEVulkan();
	case GraphicsAPI::OpenGL:
		return new Graphics::OpenGL::CEOpenGL();
	case GraphicsAPI::Metal:
		return new Graphics::Metal::CEMetal();

	case GraphicsAPI::Count:
		return nullptr;
	default:
		return nullptr;
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
		return new Windows::CEWindows();
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
