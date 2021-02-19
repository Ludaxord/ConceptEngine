#include "CEGame.h"

#include <spdlog/spdlog.h>


#include "CEConsole.h"
using namespace ConceptEngineFramework::Game;

static CEGame* g_pGame = nullptr;

/*
 * Instance for std::shared_ptr
 */
class CEConsoleInstance final : public CEConsole {

public:
	CEConsoleInstance(const std::wstring& windowName, int maxFileSizeInMB = 5, int maxFiles = 3,
	                  int maxConsoleLines = 500)
		: CEConsole(windowName, maxFileSizeInMB, maxFiles, maxConsoleLines) {
	}

	explicit CEConsoleInstance(const Logger& logger)
		: CEConsole(logger) {
	}
};

CEGame::CEGame(std::wstring name, HINSTANCE hInst) {
	m_console = std::make_shared<CEConsoleInstance>(name);
	m_console->Create();
}

CEGame& CEGame::Create(std::wstring name, HINSTANCE hInst) {
	if (!g_pGame) {
		g_pGame = new CEGame(name, hInst);
		spdlog::info("ConceptEngineFramework Game class created.");
	}

	return *g_pGame;
}

CEGame& CEGame::Get() {
	assert(g_pGame != nullptr);
	return *g_pGame;
}

uint32_t CEGame::Run() {
	assert(!m_bIsRunning);
	m_bIsRunning = true;

	MSG msg = {};
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		// m_inputManager.HandleMessage(msg);

		/*
		 * Check to see if application send quit signal
		 */
		if (m_requestQuit) {
			::PostQuitMessage(0);
			m_requestQuit = false;
		}
	}

	m_bIsRunning = false;
	return static_cast<int32_t>(msg.wParam);
}


std::shared_ptr<CEConsole> CEGame::GetConsole() {
	return m_console;
}

std::shared_ptr<CEWindow> CEGame::CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight) {
	return nullptr;
}
