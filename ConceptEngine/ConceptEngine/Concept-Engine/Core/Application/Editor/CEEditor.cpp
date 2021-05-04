#include "CEEditor.h"

using namespace ConceptEngine::Core::Application::Editor;

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                   Generic::Platform::Platform platform): CECore(api, language, platform) {
}

CEEditor::CEEditor(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform,
                   Graphics::Main::Common::CEPlayground* playground): CECore(api, language, platform, playground) {
}

CEEditor::~CEEditor() {
}

bool CEEditor::Init() {
	if (!CECore::Init()) {
		return false;
	}
	return true;
}

int CEEditor::Run() {
	return 0;
}
