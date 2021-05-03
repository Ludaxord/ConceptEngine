#pragma once

#include "../../Graphics/Main/CEGraphics.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"

namespace ConceptEngine::Core::Application {
	class CECore {
	public:
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		virtual ~CECore() = default;

		virtual bool Init() = 0;
		virtual int Run() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;

	protected:
		Graphics::Main::CEGraphics* m_graphics;
		Compilers::CECompiler* m_compiler;
		Generic::Platform::CEPlatform* m_platform;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Generic::Platform::CEPlatform* SelectPlatform(Generic::Platform::Platform platform);
	};
}
