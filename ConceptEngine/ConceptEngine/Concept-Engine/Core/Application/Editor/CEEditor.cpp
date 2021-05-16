#include "CEEditor.h"

#include "../../Debug/CEProfiler.h"

using namespace ConceptEngine::Core::Application::Editor;

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                   Generic::Platform::Platform platform): CECore(api, language, platform) {
	EnableDebug = true;
}

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
                   Graphics::Main::Common::CEPlayground* playground): CECore(api, language, platform, playground) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Create() {
	if (!CECore::Create()) {
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

	GetPlayground()->Update(DeltaTime);
	Debug::CEProfiler::Update();
	Graphics->Update(DeltaTime);
}

bool CEEditor::Release() {
	return true;
}
