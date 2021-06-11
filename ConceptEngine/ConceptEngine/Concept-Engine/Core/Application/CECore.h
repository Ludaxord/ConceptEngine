#pragma once

namespace ConceptEngine {namespace Core {namespace Generic {namespace Platform {
	enum class Platform;
	class CEPlatform;
}}}}

namespace ConceptEngine {namespace Graphics {namespace Main {namespace Common {
		class CEPlayground;
	}

	class CEGraphics;
}}}

namespace ConceptEngine {namespace Time {
	class CETimestamp;
}}

namespace ConceptEngine {namespace Core {namespace Compilers {
	struct CECompiler;
	enum class Language;
}}}

enum class GraphicsAPI;

namespace ConceptEngine::Core::Application {

	class CECore {
	public:
		CECore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform);
		CECore(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
		       Graphics::Main::Common::CEPlayground* playground);
		virtual ~CECore() = default;

		virtual bool Create();
		virtual void Run() = 0;
		virtual void Update(Time::CETimestamp DeltaTime) = 0;
		virtual bool Release() = 0;

		// static Graphics::Main::CEGraphics* GetGraphics();
		// static Compilers::CECompiler* GetCompiler();
		// static Generic::Platform::CEPlatform* GetPlatform();
		// static Graphics::Main::Common::CEPlayground* GetPlayground();

		inline static bool ShowConsole = false;
		inline static bool IsRunning = false;
		inline static bool EnableDebug = false;

	protected:
		// static Graphics::Main::CEGraphics* Graphics;
		// static Compilers::CECompiler* Compiler;
		// static Generic::Platform::CEPlatform* Platform;

	private:
		Graphics::Main::CEGraphics* SelectGraphicsAPI(GraphicsAPI api);
		Compilers::CECompiler* SelectLanguageCompiler(Compilers::Language language);
		Core::Generic::Platform::CEPlatform * SelectPlatform(Generic::Platform::Platform platform);
	};
}
