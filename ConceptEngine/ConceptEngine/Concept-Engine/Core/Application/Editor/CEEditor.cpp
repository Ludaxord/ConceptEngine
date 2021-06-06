#include "CEEditor.h"

#include "../../Debug/CEProfiler.h"

#include "../../Debug/CEDebug.h"

using namespace ConceptEngine::Core::Application::Editor;

//TODO: Cast with QT framework.

CEEditor::CEEditor(GraphicsAPI api, Compilers::Language language,
                   Generic::Platform::Platform platform): CECore(api, language, platform) {
	EnableDebug = true;
}

CEEditor::CEEditor(GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
                   Graphics::Main::Common::CEPlayground* playground): CECore(api, language, platform, playground) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Create() {
	if (!CECore::Create()) {
		Debug::CEDebug::DebugBreak();
		return false;
	}

	return true;
}

int CEEditor::Run() {
	Timer = Time::CETimer();
	return 0;
}

void CEEditor::Update(Time::CETimestamp DeltaTime) {
	Platform->Update();
	Graphics->Update(DeltaTime, [] {
		Debug::CEProfiler::Update();
	});
}

bool CEEditor::Release() {
	return true;
}
