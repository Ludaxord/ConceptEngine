#pragma once

#include "../../../Core/Common/CEManager.h"

namespace ConceptEngine::Graphics::Main::Managers {
	class CEGraphicsManager : public Core::Common::CEManager {
	public:
		CEGraphicsManager(): CEManager() {
		};
		~CEGraphicsManager() override = default;

		virtual bool Create() override;
		virtual void Destroy() = 0;
	};
}
