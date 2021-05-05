#include "CEGameCore.h"
#include "../../Time/CETimer.h"

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
	Platform->CreateSystemWindow();
	if (!Generic::Platform::CEPlatform::GetWindow()->Create()) {
		return false;
	}

	return true;
}

int Application::CEGameCore::Run() {
	Time::CETimer Timer;
	while (m_isRunning) {
		Timer.Update();
		Update(Timer.GetDeltaTime());
	}
	return 0;
}
