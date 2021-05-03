#include "CEGameDebug.h"

using namespace ConceptEngine::Core::Application::GameDebug;

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language, Platform::Platform platform): CEGameCore(api, language, platform) {
}

CEGameDebug::~CEGameDebug() {
}

int CEGameDebug::Run() {
	return CEGameCore::Run();
}

void CEGameDebug::Update() {
	CEGameCore::Update();
}

void CEGameDebug::Render() {
	CEGameCore::Render();
}
