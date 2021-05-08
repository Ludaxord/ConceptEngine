#pragma once

#include "../../Graphics/Main/CEGraphics.h"
#include "../../Graphics/Main/Common/CEPlayground.h"
#include "../Compilers/CECompiler.h"
#include "../Platform/Generic/CEPlatform.h"
#include "../Platform/CEPlatformActions.h"

namespace ConceptEngine::Core::Application {
	class CECore {
	public:
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CECore(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		virtual ~CECore() = default;

		virtual bool Create();
		virtual int Run() = 0;
		virtual void Update(Time::CETimestamp DeltaTime) = 0;
		virtual bool Release() = 0;

		static Graphics::Main::CEGraphics* GetGraphics();
		static Compilers::CECompiler* GetCompiler();
		static Generic::Platform::CEPlatform* GetPlatform();
		static Graphics::Main::Common::CEPlayground* GetPlayground();

		static int ShowConsole;

	protected:
		static Graphics::Main::CEGraphics* Graphics;
		static Compilers::CECompiler* Compiler;
		static Generic::Platform::CEPlatform* Platform;

		bool IsRunning = false;
	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(Graphics::Main::GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Generic::Platform::CEPlatform* SelectPlatform(Generic::Platform::Platform platform);
	};
}
