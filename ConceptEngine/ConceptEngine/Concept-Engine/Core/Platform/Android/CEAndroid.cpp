#include "CEAndroid.h"

using namespace ConceptEngine::Core::Platform::Android;

CEAndroid::CEAndroid() : CEPlatform() {
}

CEAndroid::~CEAndroid() {
}

void CEAndroid::Create() {
}

void CEAndroid::CreateSystemWindow() {
}

void CEAndroid::CreateSystemConsole() {
}

void CEAndroid::CreateCursors() {
}

bool CEAndroid::CreateInputManager() {
	return true;
}
