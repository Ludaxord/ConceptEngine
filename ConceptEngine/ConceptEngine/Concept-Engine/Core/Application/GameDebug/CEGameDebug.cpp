#include "CEGameDebug.h"

#include "../../../Elements/CECoreElements.h"
#include "../../Platform/CEPlatformActions.h"
#include "../../Debug/CEDebug.h"
#include "../../Debug/CEProfiler.h"
#include "../../Platform/Generic/CEPlatform.h"
#include "../../Platform/Generic/Debug/CETypedConsole.h"

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
	if (!GPlatform->CreateSystemConsole()) {
		CEPlatformActions::MessageBox("Error", "Failed to Create System Debug Console");
		return false;
	}

	ConceptEngine::Core::Generic::Platform::CEPlatform::GetConsole()->SetTitle("Concept Engine Debug Console");

	if (!Generic::Platform::CEPlatform::GetConsole()->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CEGameCore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEGameDebug::Update(Time::CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		Generic::Platform::CEPlatform::GetConsole()->Update();
		GTypedConsole.Update();
		Debug::CEProfiler::Update();
	});
}
