#include "CEEditor.h"

using namespace ConceptEngine::Core::Application::Editor;

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Platform::Platform platform): CECore(api, language, platform) {
}

CEEditor::~CEEditor() {
}

int CEEditor::Run() {
	return 0;
}

void CEEditor::Update() {
}

void CEEditor::Render() {
}
