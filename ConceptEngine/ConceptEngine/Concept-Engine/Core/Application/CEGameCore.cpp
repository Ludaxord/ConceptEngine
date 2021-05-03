#include "CEGameCore.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(Graphics::Main::GraphicsAPI api,
                                    Compilers::Language language,
                                    Generic::Platform::Platform platform): CECore(api, language, platform) {
}

bool Application::CEGameCore::Init() {
	return false;
}

int Application::CEGameCore::Run() {
	return 0;
}

void Application::CEGameCore::Update() {
}

void Application::CEGameCore::Render() {
}
