#pragma once

#include <memory>
#include <string>
#include <Windows.h>

#include "Core/CECore.h"

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
		ConceptEngine(std::wstring name, HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, int width, int height, bool showConsole);
		ConceptEngine(std::wstring name, HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, bool showConsole);

		/*
		 * Editor Constructors
		 */
		ConceptEngine(std::wstring name, HWND hWnd, int width, int height);
		
		int Run() const;

		Core::CECore* GetCore() const;
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

		std::unique_ptr<Core::CECore> m_core;
	};
}
