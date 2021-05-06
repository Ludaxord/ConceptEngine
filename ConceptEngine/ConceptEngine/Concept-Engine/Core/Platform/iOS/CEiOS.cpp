#include "CEiOS.h"

#include "../Generic/Input/CEInputManager.h"

using namespace ConceptEngine::Core::Platform::iOS;

CEiOS::CEiOS(): CEPlatform() {
}

CEiOS::~CEiOS() {
}

void CEiOS::Create() {
}

void CEiOS::CreateSystemWindow() {
}

void CEiOS::CreateSystemConsole() {
}

void CEiOS::CreateCursors() {
}

void CEiOS::Update() {
}

bool CEiOS::CreateInputManager() {
	return true;
}
