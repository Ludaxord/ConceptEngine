#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "Core/Application/CECore.h"

namespace ConceptEngine {

	enum class EngineBoot {
		Game,
		Editor,
		GameDebug
	};

	class ConceptEngine {
	public:
		ConceptEngine(
			std::wstring name,
			GraphicsAPI api,
			Core::Generic::Platform::Platform platform,
			Core::Compilers::Language language = Core::Compilers::Language::None
		);
		ConceptEngine(
			std::wstring name,
			GraphicsAPI api,
			Core::Generic::Platform::Platform platform
		);

		bool Init() const;
		int Run() const;
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
		static std::wstring Name;
	};
}
