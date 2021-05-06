#pragma once
#include "CEScreen.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	class CEConsole : public CEScreen {
	public:
		CEConsole();
		~CEConsole();

		virtual void Update() = 0;
	};
}
