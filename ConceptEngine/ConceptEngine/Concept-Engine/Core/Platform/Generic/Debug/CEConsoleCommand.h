#pragma once
#include "CEConsoleObject.h"

namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEConsoleCommand : public CEConsoleObject {
	public:
		virtual CEConsoleCommand* AsCommand() override {
			return this;
		}

		void Execute() {
			OnExecute.Broadcast();
		}

		CEMulticastDelegate<void> OnExecute;
	};
}
