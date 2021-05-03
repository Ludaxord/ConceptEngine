#include "CEEditor.h"

using namespace ConceptEngine::Core::Application::Editor;

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                   Generic::Platform::Platform platform): CECore(api, language, platform) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Init() {
	m_platform->Create();
	return true;
}

int CEEditor::Run() {
	return 0;
}
