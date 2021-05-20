#pragma once
#include "CEResource.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEViewport : public CEResource {
	public:
		virtual bool Resize(uint32 width, uint32 uint32) = 0;
	};
}
