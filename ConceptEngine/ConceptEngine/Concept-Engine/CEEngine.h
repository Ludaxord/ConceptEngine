#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "Core/Platform/CEPlatformActions.h"

#include "Core/Compilers/CECompiler.h"

enum class GraphicsAPI;

namespace ConceptEngine {
	enum class EngineBoot;

	namespace Core {namespace Application {
		class CECore;
	}}

	namespace Core {namespace Generic {namespace Platform {
		class CEPlatform;
		enum class Platform;
	}}}

	class CEEngine {
	public:
		CEEngine(
			std::wstring name,
			GraphicsAPI api,
			Core::Generic::Platform::Platform platform,
			Core::Compilers::Language language = Core::Compilers::Language::None
		);
		CEEngine(
			std::wstring name,
			GraphicsAPI api,
			Core::Generic::Platform::Platform platform
		);

		bool Init() const;
		void Run() const;
		bool Release();

		Core::Application::CECore* GetCore() const;

		static std::wstring GetName();

	protected:
		bool Create(EngineBoot boot);

		bool CreateEditor();
		bool CreateGame();
		bool CreateGameDebug();

	private:
		GraphicsAPI EnumApi;
		Core::Compilers::Language EnumLanguage;
		Core::Generic::Platform::Platform EnumPlatform;

		std::unique_ptr<Core::Application::CECore> Core;

		friend class Core::Generic::Platform::CEPlatform;

	};

	inline int RuntimeExec(ConceptEngine::CEEngine* conceptEngine) {
		if (!conceptEngine->Init()) {
			CEPlatformActions::MessageBox("Error", "Concept Engine Run -> Failed");
			return -1;
		}

		conceptEngine->Run();

		if (!conceptEngine->Release()) {
			CEPlatformActions::MessageBox("Error", "Concept Engine Release -> Failed");
			return -1;
		}

		return 0;
	}
}
