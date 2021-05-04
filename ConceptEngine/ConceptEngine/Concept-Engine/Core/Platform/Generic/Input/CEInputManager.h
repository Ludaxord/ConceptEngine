#pragma once

#include "../../../Common/CEManager.h"

namespace ConceptEngine::Core::Platform::Generic::Input {
	class CEInputManager : public Common::CEManager {
	public:
		CEInputManager();
		~CEInputManager() override;
		
		virtual bool Create() override;
	};
}
