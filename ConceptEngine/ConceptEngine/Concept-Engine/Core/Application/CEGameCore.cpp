#include "CEGameCore.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(Graphics::Main::GraphicsAPI api,
                                    Compilers::Language language,
                                    Generic::Platform::Platform platform): CECore(api, language, platform) {
}

bool Application::CEGameCore::Init() {
	m_platform->Create();
	m_platform->CreateSystemWindow();
	return true;
}

int Application::CEGameCore::Run() {
	return 0;
}
