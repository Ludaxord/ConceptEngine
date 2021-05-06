#include "CEGame.h"

#include "../../../Time/CETimer.h"
#include "../../Debug/CEProfiler.h"

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

void CEGame::Update(Time::CETimestamp DeltaTime) {
	Platform->Update();
	GetPlayground()->Update(DeltaTime);
	Debug::CEProfiler::Update();
	Graphics->Update(DeltaTime);
}
