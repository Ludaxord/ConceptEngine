#include "CEPlatform.h"

using namespace ConceptEngine::Core::Generic::Platform;
using namespace ConceptEngine::Core::Platform::Generic::Callbacks;


bool ::CEPlatform::Create() {
	return true;
}

void ::CEPlatform::SetCallbacks(CEPlatformCallbacks* InCallbacks) {
	Callbacks = InCallbacks;
}

CEPlatformCallbacks* ::CEPlatform::GetCallbacks() {
	return Callbacks;
}

CEWindow* ::CEPlatform::GetWindow() {
	return Window;
}

CEConsole* ::CEPlatform::GetConsole() {
	return Console;
}

CEInputManager* ::CEPlatform::GetInputManager() {
	return InputManager;
}
