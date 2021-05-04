#pragma once

#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Managers {
	class CETextureManager : Core::Common::CEManager {
	public:
		CETextureManager();

		virtual bool Create();
	};
}