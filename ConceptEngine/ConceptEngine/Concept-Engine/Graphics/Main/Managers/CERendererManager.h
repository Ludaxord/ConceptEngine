#pragma once

#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Managers {
	class CERendererManager : public Core::Common::CEManager {
	public:
		CERendererManager();
		~CERendererManager() override;

		bool Create() override {
			return true;
		}
	};
}
	