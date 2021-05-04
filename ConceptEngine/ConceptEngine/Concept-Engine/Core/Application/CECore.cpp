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

using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Core::Compilers;
using namespace ConceptEngine::Core::Platform;

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Language language,
                                                 Generic::Platform::Platform platform) {
	m_graphics = SelectGraphicsAPI(api);
	m_compiler = SelectLanguageCompiler(language);
	m_platform = SelectPlatform(platform);
}

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Compilers::Language language,
                                                 Generic::Platform::Platform platform,
                                                 Graphics::Main::Common::CEPlayground* playground) :
	CECore(api, language, platform) {
	m_playground = playground;
}

bool ConceptEngine::Core::Application::CECore::Init() {
	Debug::CEProfiler::Init();

	m_platform->Create();

	if (!Threading::CETaskManager::Get().Create()) {
		return false;
	}

	if (!m_graphics->Create()) {
		return false;
	}

	return true;
}

CEGraphics* ConceptEngine::Core::Application::CECore::GetGraphics() {
	return m_graphics;
}

CECompiler* ConceptEngine::Core::Application::CECore::GetCompiler() {
	return m_compiler;
}

ConceptEngine::Core::Generic::Platform::CEPlatform* ConceptEngine::Core::Application::CECore::GetPlatform() {
	return m_platform;
}

Common::CEPlayground* ConceptEngine::Core::Application::CECore::GetPlayground() {
	return m_playground;
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
