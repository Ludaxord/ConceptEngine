#include "CEGameDebug.h"

#include "../../Debug/CEProfiler.h"

using namespace ConceptEngine::Core::Application::GameDebug;

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                         Generic::Platform::Platform platform): CEGameCore(api, language, platform) {
}

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                         Generic::Platform::Platform platform,
                         Graphics::Main::Common::CEPlayground* playground): CEGameCore(
	api, language, platform, playground) {
}

CEGameDebug::~CEGameDebug() {
}

bool CEGameDebug::Create() {
	if (!Platform->CreateSystemConsole()) {
		CEPlatformActions::MessageBox("Error", "Failed to Create System Debug Console");
		return false;
	}

	if (!CEGameCore::Create()) {
		return false;
	}

	if (!Generic::Platform::CEPlatform::GetConsole()->Create()) {
		return false;
	}

	return true;
}

void CEGameDebug::Update(Time::CETimestamp DeltaTime) {
	Platform->Update();
	GetPlayground()->Update(DeltaTime);
	Generic::Platform::CEPlatform::GetConsole()->Update();
	Debug::CEProfiler::Update();
	Graphics->Update(DeltaTime);
}
