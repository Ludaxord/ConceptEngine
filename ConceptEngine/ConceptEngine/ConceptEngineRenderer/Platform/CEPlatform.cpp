#include "CEPlatform.h"

bool CEPlatform::Create() {
	ProjectConfig.Title = GEngineConfig.InstanceName;
	return true;
}

void CEPlatform::SetCallbacks(CEPlatformCallbacks* InCallbacks) {
	Callbacks = InCallbacks;
}

CEPlatformCallbacks* CEPlatform::GetCallbacks() {
	return Callbacks;
}

CEWindow* CEPlatform::GetWindow() {
	return Window;
}

CEConsole* CEPlatform::GetConsole() {
	return Console;
}

CEInputManager* CEPlatform::GetInputManager() {
	return InputManager;
}
