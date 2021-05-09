#pragma once
namespace ConceptEngine::Core::Platform::Generic::Debug {
	class CEConsoleCommand;
	class CEConsoleVariable;

	class CEConsoleObject {
	public:
		virtual ~CEConsoleObject() = default;

		virtual CEConsoleCommand* AsCommand() {
			return nullptr;
		}

		virtual CEConsoleVariable* AsVariable() {
			return nullptr;
		}
	};
}
