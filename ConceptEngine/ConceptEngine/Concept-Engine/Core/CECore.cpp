#include "CECore.h"

#include "../Graphics/DirectX12/CEDirectX12.h"
#include "../Graphics/Vulkan/CEVulkan.h"
#include "../Graphics/OpenGL/CEOpenGL.h"
#include "../Graphics/Metal/CEMetal.h"

#include "../Compilers/Cpp/CECppCompiler.h"
#include "../Compilers/CSharp/CECSCompiler.h"
#include "../Compilers/JS/CEJSCompiler.h"
#include "../Compilers/Python/CEPyCompiler.h"
#include "../Compilers/Schematics/CESchematicsCompiler.h"

using namespace ConceptEngine::Graphics::Main;
using namespace ConceptEngine::Compilers;

ConceptEngine::Core::CECore::CECore(GraphicsAPI api, Language language) {
	m_graphics = SelectGraphicsAPI(api);
	m_compiler = SelectLanguageCompiler(language);
}

CEGraphics* ConceptEngine::Core::CECore::SelectGraphicsAPI(GraphicsAPI api) {
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

CECompiler* ConceptEngine::Core::CECore::SelectLanguageCompiler(Compilers::Language language) {
	switch (language) {
	case Compilers::Language::Cpp:
		return new Compilers::Cpp::CECppCompiler();
	case Compilers::Language::CSharp:
		return new Compilers::CSharp::CECSCompiler();
	case Compilers::Language::JS:
		return new Compilers::JS::CEJSCompiler();
	case Compilers::Language::Python:
		return new Compilers::Python::CEPyCompiler();
	case Compilers::Language::Schematics:
		return new Compilers::Schematics::CESchematicsCompiler();
	case Compilers::Language::Count:
		return nullptr;
	case Compilers::Language::None:
		return nullptr;
	default:
		return nullptr;
	}
}
