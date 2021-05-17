#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Core::Common {
	enum class CEManagerType {
		GraphicsManager = 0,
		TextureManager = 1,
		RendererManager = 2
	};

	class CEManager {
	public:
		CEManager() {
		};
		virtual ~CEManager() = default;
		virtual bool Create() = 0;
	};
}
