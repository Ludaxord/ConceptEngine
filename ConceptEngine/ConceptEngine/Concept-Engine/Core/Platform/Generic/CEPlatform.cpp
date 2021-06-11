#include "CEPlatform.h"

#include "../../../Utilities/CEUtilities.h"
#include "Callbacks/CEPlatformCallbacks.h"


bool ConceptEngine::Core::Generic::Platform::CEPlatform::Create() {
	Title = InstanceName;
	return true;
}

void ConceptEngine::Core::Generic::Platform::CEPlatform::SetCallbacks(ConceptEngine::Core::Platform::Generic::Callbacks::CEPlatformCallbacks* InCallbacks) {
	Callbacks = InCallbacks;
}

ConceptEngine::Core::Platform::Generic::Callbacks::CEPlatformCallbacks* ConceptEngine::Core::Generic::Platform::CEPlatform::GetCallbacks() {
	return Callbacks;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* ConceptEngine::Core::Generic::Platform::CEPlatform::GetWindow() {
	return Window;
}

ConceptEngine::Core::Platform::Generic::Window::CEConsole* ConceptEngine::Core::Generic::Platform::CEPlatform::GetConsole() {
	return Console;
}

ConceptEngine::Core::Platform::Generic::Input::CEInputManager* ConceptEngine::Core::Generic::Platform::CEPlatform::GetInputManager() {
	return InputManager;
}
