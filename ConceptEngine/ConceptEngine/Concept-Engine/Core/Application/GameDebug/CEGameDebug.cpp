#include "CEGameDebug.h"

using namespace ConceptEngine::Core::Application::GameDebug;

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                         Generic::Platform::Platform platform): CEGameCore(api, language, platform) {
}

CEGameDebug::CEGameDebug(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                         Generic::Platform::Platform platform,
                         Graphics::Main::Common::CEPlayground* playground): CEGameCore(
	api, language, platform, playground) {
}

CEGameDebug::~CEGameDebug() {
}

bool CEGameDebug::Create() {
	if (!CEGameCore::Create()) {
		return false;
	}

	m_platform->CreateSystemConsole();

	if (!Generic::Platform::CEPlatform::GetConsole()->Create()) {
		return false;
	}

	return true;
}

int CEGameDebug::Run() {
	return CEGameCore::Run();
}
