#pragma once

#include "Window/CEConsole.h"
#include "Window/CEWindow.h"

namespace ConceptEngine::Core::Generic::Platform {
	enum class Platform {
		Android,
		iOS,
		Linux,
		Mac,
		Windows,
		Unknown
	};

	class CEPlatform {
	public:
		friend class CECore;
		CEPlatform() = default;
		virtual ~CEPlatform() = default;

		virtual void Create() = 0;
		virtual void CreateSystemWindow() = 0;
		virtual void CreateSystemConsole() = 0;
		virtual void CreateCursors() = 0;

	protected:
		Core::Platform::Generic::Window::CEWindow* m_window;
		Core::Platform::Generic::Window::CEConsole* m_console;
	};
}
