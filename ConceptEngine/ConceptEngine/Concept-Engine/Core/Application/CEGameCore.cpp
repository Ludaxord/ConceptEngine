#include "CEGameCore.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(Graphics::Main::GraphicsAPI api,
                                    Compilers::Language language,
                                    Generic::Platform::Platform platform): CECore(api, language, platform) {
}

Application::CEGameCore::CEGameCore(Graphics::Main::GraphicsAPI api, Compilers::Language language,
                                    Generic::Platform::Platform platform,
                                    Graphics::Main::Common::CEPlayground* playground): CECore(
	api, language, platform, playground) {
}

bool Application::CEGameCore::Create() {
	if (!CECore::Create()) {
		return false;
	}
	m_platform->CreateSystemWindow();
	if (!Generic::Platform::CEPlatform::GetWindow()->Create()) {
		return false;
	}
	return true;
}

int Application::CEGameCore::Run() {
	return 0;
}
