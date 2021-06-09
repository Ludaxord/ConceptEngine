#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "Core/Application/Game/CEGame.h"
#include "Core/Application/Editor/CEEditor.h"
#include "Core/Application/GameDebug/CEGameDebug.h"
#include "Core/Platform/CEPlatformActions.h"
#include "Core/Platform/Windows/CEWindows.h"
#include "Graphics/Main/Common/API.h"
#include "Utilities/CEScreenUtilities.h"

namespace ConceptEngine {

	enum class EngineBoot {
		Game,
		Editor,
		GameDebug
	};

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

		static EngineBoot GetEngineBoot();
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
		static std::wstring Name;
		static EngineBoot EnumEngineBoot;

	};

	inline int RuntimeRun(ConceptEngine::CEEngine* conceptEngine) {
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
