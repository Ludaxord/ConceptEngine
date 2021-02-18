#include "CEGame.h"

#include <spdlog/spdlog.h>


#include "CEConsole.h"
using namespace ConceptEngineFramework::Game;

static CEGame* g_pGame = nullptr;

CEGame::CEGame(std::wstring name, HINSTANCE hInst) {
	auto* console = new CEConsole(name);
	console->Create();
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