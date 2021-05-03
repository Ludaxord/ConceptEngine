#include "CEGameDebug.h"

using namespace ConceptEngine::Core::Application::GameDebug;

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language, Generic::Platform::Platform platform): CEGameCore(api, language, platform) {
}

CEGameDebug::~CEGameDebug() {
}

bool CEGameDebug::Init() {
	return CEGameCore::Init();
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
