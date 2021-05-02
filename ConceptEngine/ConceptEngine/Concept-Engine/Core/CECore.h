#pragma once
#include <memory>

#include "../Graphics/Main/CEGraphics.h"
#include "../Compilers/CECompiler.h"

namespace ConceptEngine::Core {
	class CECore {
	public:
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language);
		virtual ~CECore() = default;

		virtual int Run() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;

	protected:
		Graphics::Main::CEGraphics* m_graphics;
		Compilers::CECompiler* m_compiler;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
	};
}
