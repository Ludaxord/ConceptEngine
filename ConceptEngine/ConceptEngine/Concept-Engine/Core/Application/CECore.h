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

		static Graphics::Main::CEGraphics* GetGraphics();
		static Compilers::CECompiler* GetCompiler();
		static Generic::Platform::CEPlatform* GetPlatform();
		static Graphics::Main::Common::CEPlayground* GetPlayground();

	protected:
		static Graphics::Main::CEGraphics* m_graphics;
		static Compilers::CECompiler* m_compiler;
		static Generic::Platform::CEPlatform* m_platform;
		inline static Graphics::Main::Common::CEPlayground* m_playground = nullptr;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Generic::Platform::CEPlatform* SelectPlatform(Generic::Platform::Platform platform);
	};
}
