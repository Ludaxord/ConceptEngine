#pragma once

#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Managers {
	class CETextureManager : public Core::Common::CEManager {
	public:
		CETextureManager();
		~CETextureManager() override;
		virtual bool Create() override;
	};
}