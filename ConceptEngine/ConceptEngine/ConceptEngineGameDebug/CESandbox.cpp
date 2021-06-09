#include "CESandbox.h"

ConceptEngine::Graphics::Main::Common::CEPlayground* CreatePlayground() {
	return new CESandbox();
}

bool CESandbox::Create() {
	if (!ConceptEngine::Graphics::Main::Common::CEPlayground::Create()) {
		return false;
	}

	return true;
}

void CESandbox::Update(ConceptEngine::Time::CETimestamp DeltaTime) {
}