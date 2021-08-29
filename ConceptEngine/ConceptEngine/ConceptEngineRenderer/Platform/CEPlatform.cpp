#include "CEPlatform.h"

bool CEPlatform::Create() {
	PlatformConfig.PTitle = GEngineConfig.InstanceName;
	return true;
}

void CEPlatform::SetCallbacks(CEPlatformCallbacks* InCallbacks) {
	Callbacks = InCallbacks;
}

CEPlatformCallbacks* CEPlatform::GetCallbacks() {
	return Callbacks;
}

CEConsole* CEPlatform::GetConsole() {
	return Console;
}

CEInputManager* CEPlatform::GetInputManager() {
	return InputManager;
}
