#include "CEPlatform.h"

using namespace ConceptEngine::Core::Generic::Platform;
using namespace ConceptEngine::Core::Platform::Generic::Callbacks;


void CEPlatform::Create() {
}

void CEPlatform::SetCallbacks(CEPlatformCallbacks* InCallbacks) {
	Callbacks = InCallbacks;
}

CEPlatformCallbacks* CEPlatform::GetCallbacks() {
	return Callbacks;
}
