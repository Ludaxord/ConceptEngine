#pragma once

#include "../../Generic/Input/CEInputManager.h"

namespace ConceptEngine::Core::Platform::Windows::Input {
	class CEWindowsInputManager final : public Generic::Input::CEInputManager {
	public:
		CEWindowsInputManager();
		
		~CEWindowsInputManager() override;
		bool Create() override;
	};
}
