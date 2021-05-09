#pragma once
#include "CEConsoleObject.h"
#include "../../../Delegates/CEMulticastDelegate.h"

namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEConsoleCommand : public CEConsoleObject {
	public:
		virtual CEConsoleCommand* AsCommand() override {
			return this;
		}

		void Execute() {
			OnExecute.Broadcast();
		}

		Delegates::CEMulticastDelegate<void> OnExecute;
	};
}
