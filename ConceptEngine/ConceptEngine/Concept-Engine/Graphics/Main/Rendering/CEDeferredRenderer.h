#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEDeferredRenderer {
	public:
		CEDeferredRenderer() = default;
		~CEDeferredRenderer() = default;


		bool Create(CEFrameResources& FrameResources);
	};
}
