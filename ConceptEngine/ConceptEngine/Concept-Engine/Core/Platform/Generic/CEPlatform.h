#pragma once
#include <string>

#include "Input/CEModifierKeyState.h"
#include "Input/CEInputManager.h"
#include "../../Containers/CEArray.h"
#include "Callbacks/CEPlatformCallbacks.h"
#include "Cursor/CECursor.h"
#include "Events/CEEvent.h"
#include "Window/CEConsole.h"


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
		CEPlatform() = default;
		virtual ~CEPlatform() = default;

		virtual bool Create();
		virtual bool CreateSystemWindow() = 0;
		virtual bool CreateSystemConsole() = 0;
		virtual bool CreateCursors() = 0;

		virtual bool CreateManagers() {
			if (!CreateInputManager()) {
				return false;
			}
			InputManager->Create();

			return true;
		}

		virtual void Update() = 0;
		virtual bool Release() = 0;

		virtual Core::Platform::Generic::Input::CEModifierKeyState GetModifierKeyState() {
			return Core::Platform::Generic::Input::CEModifierKeyState();
		};

		virtual void SetCapture(Core::Platform::Generic::Window::CEWindow* window) = 0;
		virtual void SetActiveWindow(Core::Platform::Generic::Window::CEWindow* window) = 0;

		virtual Core::Platform::Generic::Window::CEWindow* GetCapture() = 0;
		virtual Core::Platform::Generic::Window::CEWindow* GetActiveWindow() = 0;

		virtual void SetCursor(Core::Platform::Generic::Cursor::CECursor* cursor) = 0;
		virtual Core::Platform::Generic::Cursor::CECursor* GetCursor() = 0;

		virtual void SetCursorPosition(Core::Platform::Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) = 0;
		virtual void GetCursorPosition(Core::Platform::Generic::Window::CEWindow* relativeWindow, int32& x, int32& y) = 0;

		virtual void SetCallbacks(Core::Platform::Generic::Callbacks::CEPlatformCallbacks* InCallbacks);

		static Core::Platform::Generic::Callbacks::CEPlatformCallbacks* GetCallbacks();
		static Core::Platform::Generic::Window::CEWindow* GetWindow();
		static Core::Platform::Generic::Window::CEConsole* GetConsole();
		static Core::Platform::Generic::Input::CEInputManager* GetInputManager();

	private:
		virtual bool CreateInputManager() = 0;

	public:
		inline static std::wstring Title;

		inline static Core::Platform::Generic::Cursor::CECursor* Cursor;
		inline static Core::Platform::Generic::Window::CEWindow* Window;
		inline static Core::Platform::Generic::Window::CEConsole* Console;
		inline static Core::Platform::Generic::Input::CEInputManager* InputManager;
		inline static Core::Platform::Generic::Callbacks::CEPlatformCallbacks* Callbacks;

		inline static CEArray<Core::Platform::Generic::Events::CEEvent> Messages;
		inline static bool IsTracingMouse;
	};
}
