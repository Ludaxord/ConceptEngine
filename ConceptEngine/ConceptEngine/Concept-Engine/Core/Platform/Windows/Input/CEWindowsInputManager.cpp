#include "CEWindowsInputManager.h"

using namespace ConceptEngine::Core::Platform::Windows::Input;

CEWindowsInputManager::CEWindowsInputManager() {
}

CEWindowsInputManager::~CEWindowsInputManager() {
}

bool CEWindowsInputManager::Create() {
	if (!CEInputManager::Create()) {
		return false;
	}

	return true;
}
