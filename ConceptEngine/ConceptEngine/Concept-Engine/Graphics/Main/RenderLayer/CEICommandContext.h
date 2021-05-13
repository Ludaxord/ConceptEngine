#pragma once

#include "../../../Core/Common/CERefCountedObject.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	class CEICommandContext : public Core::Common::CERefCountedObject {
	public:
		virtual void Begin() = 0;
		virtual void End() = 0;
	};
}
