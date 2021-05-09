#include "CEDebugUI.h"

#include <string>

#include "../../../Core/Containers/CEArray.h"

using namespace ConceptEngine::Graphics::Main::Rendering;

static ConceptEngine::Core::Containers::CEArray<CEDebugUI::UIDrawFunc> DrawFuncs;
static ConceptEngine::Core::Containers::CEArray<std::string> DebugStrings;

CEDebugUI::CEDebugUI() {
}

CEDebugUI::~CEDebugUI() {
}

void CEDebugUI::DrawUI(UIDrawFunc DrawFunc) {
	DrawFuncs.EmplaceBack(DrawFunc);
}
