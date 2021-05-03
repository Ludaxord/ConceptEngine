#include "CEGame.h"

using namespace ConceptEngine::Core::Game;

CEGame::CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language) : CEGameCore(api, language) {
}

CEGame::~CEGame() {
}

int CEGame::Run() {
	return 0;
}

void CEGame::Update() {
}

void CEGame::Render() {
}
