#pragma once
#include "CECore.h"

namespace ConceptEngine::Core {
	class CEGameCore : public CECore {
	public:
		CEGameCore(Graphics::Main::GraphicsAPI api, Compilers::Language language);
	};
}
