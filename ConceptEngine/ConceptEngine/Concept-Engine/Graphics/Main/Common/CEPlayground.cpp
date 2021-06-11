#include "CEPlayground.h"

using namespace ConceptEngine::Graphics::Main::Common;

CEPlayground* GPlayground;

CEPlayground::CEPlayground() {
}

CEPlayground::~CEPlayground() {
}

bool CEPlayground::Create() {
	return true;
}

void CEPlayground::Update(Time::CETimestamp DeltaTime) {
}

bool CEPlayground::Release() {
	return true;
}

