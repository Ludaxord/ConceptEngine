#include "CEGameCore.h"
#include "../../Time/CETimer.h"

using namespace ConceptEngine::Core;

Application::CEGameCore::CEGameCore(GraphicsAPI api,
                                    Compilers::Language language,
                                    Generic::Platform::Platform platform): CECore(api, language, platform) {
}

Application::CEGameCore::CEGameCore(GraphicsAPI api, Compilers::Language language,
                                    Generic::Platform::Platform platform,
                                    Graphics::Main::Common::CEPlayground* playground): CECore(
	api, language, platform, playground) {
}

bool Application::CEGameCore::Create() {
	if (!CECore::Create()) {
		return false;
	}
	if (!Platform->CreateSystemWindow()) {
		return false;
	}

	if (Platform->CreateCursors()) {
		return false;
	}

	return true;
}

int Application::CEGameCore::Run() {
	Time::CETimer Timer;
	while (IsRunning) {
		Timer.Update();
		Update(Timer.GetDeltaTime());
	}
	return 0;
}

bool Application::CEGameCore::Release() {
	return true;
}
