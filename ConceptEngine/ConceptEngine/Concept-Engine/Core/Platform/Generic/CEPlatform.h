#pragma once

#include "Callbacks/CEPlatformCallbacks.h"
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

		virtual void Create();
		virtual void CreateSystemWindow() = 0;
		virtual void CreateSystemConsole() = 0;
		virtual void CreateCursors() = 0;

		virtual bool CreateManagers() {
			if (!CreateInputManager()) {
				return false;
			}
			InputManager->Create();

			return true;
		}

		virtual void Update() = 0;

		static void SetCallbacks(Core::Platform::Generic::Callbacks::CEPlatformCallbacks* InCallbacks);
		static Core::Platform::Generic::Callbacks::CEPlatformCallbacks* GetCallbacks();

		static Core::Platform::Generic::Window::CEWindow* GetWindow() {
			return Window;
		}

		static Core::Platform::Generic::Window::CEConsole* GetConsole() {
			return Console;
		}

		static Core::Platform::Generic::Input::CEInputManager* GetInputManager() {
			return InputManager;
		}

	private:
		virtual bool CreateInputManager() = 0;

	protected:
		static Core::Platform::Generic::Window::CEWindow* Window;
		static Core::Platform::Generic::Window::CEConsole* Console;
		static Core::Platform::Generic::Input::CEInputManager* InputManager;
		static Core::Platform::Generic::Callbacks::CEPlatformCallbacks* Callbacks;
	};
}
