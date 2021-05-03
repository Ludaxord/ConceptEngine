#pragma once

#include "../../Graphics/Main/CEGraphics.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/CEPlatform.h"

namespace ConceptEngine::Core::Application {
	class CECore {
	public:
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Platform::Platform platform);
		virtual ~CECore() = default;

		virtual int Run() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;

	protected:
		Graphics::Main::CEGraphics* m_graphics;
		Compilers::CECompiler* m_compiler;
		Platform::CEPlatform* m_platform;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Platform::CEPlatform* SelectPlatform(Platform::Platform platform);
	};
}
