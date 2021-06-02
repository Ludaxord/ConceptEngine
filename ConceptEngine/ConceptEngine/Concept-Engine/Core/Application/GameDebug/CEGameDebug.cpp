#include "CEGameDebug.h"

#include "../../Debug/CEProfiler.h"

#include "../../Debug/CEDebug.h"

using namespace ConceptEngine::Core::Application::GameDebug;

CEGameDebug::CEGameDebug(GraphicsAPI api, Compilers::Language language,
                         Generic::Platform::Platform platform): CEGameCore(api, language, platform) {
	EnableDebug = true;
}

CEGameDebug::CEGameDebug(GraphicsAPI api, Compilers::Language language,
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

	Generic::Platform::CEPlatform::GetConsole()->SetTitle("Concept Engine Debug Console");

	if (!Generic::Platform::CEPlatform::GetConsole()->Create()) {
		Debug::CEDebug::DebugBreak();
		return false;
	}

	if (!CEGameCore::Create()) {
		Debug::CEDebug::DebugBreak();
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
