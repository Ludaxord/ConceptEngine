#include "CEGameCore.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(Graphics::Main::GraphicsAPI api,
                                    Compilers::Language language,
                                    Platform::Platform platform): CECore(api, language, platform) {
}

int Application::CEGameCore::Run() {
	return 0;
}

void Application::CEGameCore::Update() {
}

void Application::CEGameCore::Render() {
}
