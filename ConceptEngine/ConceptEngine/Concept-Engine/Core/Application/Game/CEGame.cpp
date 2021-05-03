#include "CEGame.h"

using namespace ConceptEngine::Core::Application::Game;

CEGame::CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language, Platform::Platform platform) : CEGameCore(
	api, language, platform) {
}

CEGame::~CEGame() {
}

int CEGame::Run() {
	return CEGameCore::Run();
}

void CEGame::Update() {
	CEGameCore::Update();
}

void CEGame::Render() {
	CEGameCore::Render();
}
