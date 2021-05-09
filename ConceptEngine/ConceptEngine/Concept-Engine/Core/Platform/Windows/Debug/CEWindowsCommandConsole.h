#pragma once
#include "../../Generic/Debug/CEActionConsole.h"

namespace ConceptEngine::Core::Platform::Windows::Debug {
	class CEWindowsCommandConsole : public Generic::Debug::CEActionConsole {
	public:
		void Update() override;
	};

	inline void CEWindowsCommandConsole::Update() {
	}
}
