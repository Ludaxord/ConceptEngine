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

using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Core::Compilers;
using namespace ConceptEngine::Core::Platform;

ConceptEngine::Core::Application::CECore::CECore(GraphicsAPI api, Language language, Platform::Platform platform) {
	m_graphics = SelectGraphicsAPI(api);
	m_compiler = SelectLanguageCompiler(language);
	m_platform = SelectPlatform(platform);
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

CEPlatform* ConceptEngine::Core::Application::CECore::SelectPlatform(Platform::Platform platform) {
	switch (platform) {
	case Platform::Platform::Android:
		return new Android::CEAndroid();
	case Platform::Platform::iOS:
		return new iOS::CEiOS();
	case Platform::Platform::Linux:
		return new Linux::CELinux();
	case Platform::Platform::Mac:
		return new Mac::CEMac();
	case Platform::Platform::Windows:
		return new Windows::CEWindows();
	case Platform::Platform::Unknown:
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
