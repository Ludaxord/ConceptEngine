#include "CEGame.h"

using namespace ConceptEngine::Core::Application::Game;

CEGame::CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language,
               Generic::Platform::Platform platform) : CEGameCore(
	api, language, platform) {
}

CEGame::CEGame(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
               Graphics::Main::Common::CEPlayground* playground): CEGameCore(api, language, platform, playground) {
}

CEGame::~CEGame() {
}

bool CEGame::Create() {
	if (!CEGameCore::Create()) {
		return false;
	}
	return true;
}

int CEGame::Run() {
	return CEGameCore::Run();
}
