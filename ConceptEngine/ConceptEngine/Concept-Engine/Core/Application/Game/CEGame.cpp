#include "CEGame.h"

using namespace ConceptEngine::Core::Application::Game;

CEGame::CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language,
               Generic::Platform::Platform platform) : CEGameCore(
	api, language, platform) {
}

CEGame::~CEGame() {
}

bool CEGame::Init() {
	if (!CEGameCore::Init()) {
		return false;
	}
	return true;
}

int CEGame::Run() {
	return CEGameCore::Run();
}
