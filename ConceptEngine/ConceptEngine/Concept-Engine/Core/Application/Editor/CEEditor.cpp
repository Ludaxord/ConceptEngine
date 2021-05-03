#include "CEEditor.h"

using namespace ConceptEngine::Core::Application::Editor;

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform): CECore(api, language, platform) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Init() {
	return false;
}

int CEEditor::Run() {
	return 0;
}

void CEEditor::Update() {
}

void CEEditor::Render() {
}
