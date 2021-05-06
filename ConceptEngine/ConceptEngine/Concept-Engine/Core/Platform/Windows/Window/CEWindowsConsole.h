#pragma once
#include "../../Generic/Window/CEConsole.h"

namespace ConceptEngine::Core::Platform::Windows::Window {
	class CEWindowsConsole final : public Generic::Window::CEConsole {
	public:
		CEWindowsConsole();
		~CEWindowsConsole() override;

		bool Create() override;
		void Update() override;
	protected:
	private:
	};
}
