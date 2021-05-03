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
		/*
		 * Game Constructors
		 */
		ConceptEngine(
			std::wstring name,
			HINSTANCE hInstance,
			LPSTR lpCmdLine,
			int nCmdShow,
			int width,
			int height,
			bool showConsole,
			Graphics::Main::GraphicsAPI api,
			Core::Generic::Platform::Platform platform,
			Core::Compilers::Language language = Core::Compilers::Language::None
		);
		ConceptEngine(
			std::wstring name,
			HINSTANCE hInstance,
			LPSTR lpCmdLine,
			int nCmdShow,
			bool showConsole,
			Graphics::Main::GraphicsAPI api,
			Core::Generic::Platform::Platform platform,
			Core::Compilers::Language language = Core::Compilers::Language::None
		);

		/*
		 * Editor Constructors
		 */
		ConceptEngine(
			std::wstring name,
			HWND hWnd,
			int width,
			int height,
			Graphics::Main::GraphicsAPI api,
			Core::Generic::Platform::Platform platform
		);

		bool Init();
		int Run() const;

		Core::Application::CECore* GetCore() const;
	protected:
		bool Create(EngineBoot boot);

		bool CreateEditor();
		bool CreateGame();
		bool CreateGameDebug();

	private:
		std::wstring m_name;
		HINSTANCE m_hInstance;
		LPSTR m_lpCmdLine;
		int m_nCmdShow;

		HWND m_hwnd;

		Graphics::Main::GraphicsAPI m_api;
		Core::Compilers::Language m_language;
		Core::Generic::Platform::Platform m_platform;

		std::unique_ptr<Core::Application::CECore> m_core;
	};
}
