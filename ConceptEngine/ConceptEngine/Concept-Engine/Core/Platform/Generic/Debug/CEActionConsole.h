#pragma once
#include <string>

#include "CEConsoleCommand.h"

namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEActionConsole {
	public:
		void RegisterCommand(const std::string& name, CEConsoleCommand* object);
		void RegisterVariable(const std::string& name, CEConsoleVariable* object);
	};
}
