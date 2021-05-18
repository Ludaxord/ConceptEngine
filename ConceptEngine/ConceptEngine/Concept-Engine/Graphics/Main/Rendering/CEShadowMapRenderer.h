#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEShadowMapRenderer {
	public:
		bool Create(const CELightSetup& lightSetup, const CEFrameResources& resources);
		void Release();
	};
}
