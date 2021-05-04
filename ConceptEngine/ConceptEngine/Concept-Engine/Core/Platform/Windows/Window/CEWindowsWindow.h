#pragma once

#include "../../Generic/Window/CEWindow.h"

namespace ConceptEngine::Core::Platform::Windows::Window {
	class CEWindowsWindow final : public Generic::Window::CEWindow {
	public:
		CEWindowsWindow();
		~CEWindowsWindow() override;
		
		bool Create() override;
	protected:
	private:
	};
}
