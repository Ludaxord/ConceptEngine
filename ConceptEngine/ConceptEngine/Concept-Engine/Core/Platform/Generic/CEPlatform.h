#pragma once

#include "Window/CEConsole.h"
#include "Window/CEWindow.h"
#include "Input/CEInputManager.h"

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

		virtual bool CreateManagers() {
			if (!CreateInputManager()) {
				return false;
			}
			m_inputManager->Create();

			return true;
		}

		static Core::Platform::Generic::Window::CEWindow* GetWindow() {
			return m_window;
		}

		static Core::Platform::Generic::Window::CEConsole* GetConsole() {
			return m_console;
		}

		static Core::Platform::Generic::Input::CEInputManager* GetInputManager() {
			return m_inputManager;
		}

	private:
		virtual bool CreateInputManager() = 0;

	protected:
		static Core::Platform::Generic::Window::CEWindow* m_window;
		static Core::Platform::Generic::Window::CEConsole* m_console;
		static Core::Platform::Generic::Input::CEInputManager* m_inputManager;

	};
}
