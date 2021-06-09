#include "CEGame.h"

#include "../../../Time/CETimer.h"
#include "../../Debug/CEProfiler.h"

#include "../../Debug/CEDebug.h"

#include "../../../Render/CERenderer.h"

using namespace ConceptEngine::Core::Application::Game;

CEGame::CEGame(GraphicsAPI api, Compilers::Language language,
               Generic::Platform::Platform platform) : CEGameCore(
	api, language, platform) {
	EnableDebug = false;
}

CEGame::CEGame(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
               Graphics::Main::Common::CEPlayground* playground): CEGameCore(api, language, platform, playground) {
}

CEGame::~CEGame() {
}

bool CEGame::Create() {
	if (!CEGameCore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

void CEGame::Update(Time::CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		Debug::CEProfiler::Update();
	});
}
