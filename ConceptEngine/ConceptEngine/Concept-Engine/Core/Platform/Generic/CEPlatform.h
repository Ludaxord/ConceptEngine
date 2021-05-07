#pragma once

#include "Callbacks/CEPlatformCallbacks.h"
#include "Cursor/CECursor.h"
#include "Events/CEEvent.h"
#include "Window/CEConsole.h"
#include "Window/CEWindow.h"
#include "Input/CEInputManager.h"
#include "Input/CEModifierKeyState.h"
#include "../../Containers/CEArray.h"


namespace ConceptEngine::Core::Generic::Platform {
	using namespace Core::Platform::Generic::Input;
	using namespace Core::Platform::Generic::Window;
	using namespace Core::Platform::Generic::Cursor;
	using namespace Core::Platform::Generic::Callbacks;

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
		virtual bool Release() = 0;

		virtual CEModifierKeyState GetModifierKeyState() = 0;

		virtual void SetCapture(CEWindow* window);
		virtual void SetActiveWindow(CEWindow* window);

		virtual CEWindow* GetCapture();
		virtual CEWindow* GetActiveWindow();

		virtual void SetCursor(CECursor* cursor);
		virtual CECursor* GetCursor();

		virtual void SetCursorPosition(CEWindow* relativeWindow, int32 x, int32 y);
		virtual void GetCursorPosition(CEWindow* relativeWindow, int32 x, int32 y);

		virtual void SetCallbacks(CEPlatformCallbacks* InCallbacks);

		static CEPlatformCallbacks* GetCallbacks();
		static CEWindow* GetWindow();
		static CEConsole* GetConsole();
		static CEInputManager* GetInputManager();

	private:
		virtual bool CreateInputManager() = 0;

	protected:
		static CECursor* Cursor;
		static CEWindow* Window;
		static CEConsole* Console;
		static CEInputManager* InputManager;
		static CEPlatformCallbacks* Callbacks;

		static Containers::CEArray<CEEvent> Messages;
		static bool IsTracingMouse;
	};
}
