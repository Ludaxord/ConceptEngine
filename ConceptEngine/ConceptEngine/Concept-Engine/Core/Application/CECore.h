#pragma once

#include "../../Graphics/Main/CEGraphics.h"
#include "../../Graphics/Main/Common/CEPlayground.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"

namespace ConceptEngine::Core::Application {
	class CECore {
	public:
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform, Graphics::Main::Common::CEPlayground* playground);
		virtual ~CECore() = default;

		virtual bool Init() = 0;
		virtual int Run() = 0;

		Graphics::Main::CEGraphics* GetGraphics() const;
		Compilers::CECompiler* GetCompiler() const;
		Generic::Platform::CEPlatform* GetPlatform() const;
		Graphics::Main::Common::CEPlayground* GetPlayground() const;

	protected:
		Graphics::Main::CEGraphics* m_graphics;
		Compilers::CECompiler* m_compiler;
		Generic::Platform::CEPlatform* m_platform;
		Graphics::Main::Common::CEPlayground* m_playground = nullptr;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Generic::Platform::CEPlatform* SelectPlatform(Generic::Platform::Platform platform);
	};
}
