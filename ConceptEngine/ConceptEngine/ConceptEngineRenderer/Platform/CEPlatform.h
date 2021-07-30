#pragma once
#include "Boot/Callbacks/CEPlatformCallbacks.h"
#include "Config/CEGlobalConfigs.h"
#include "Core/Application/Generic/CEGenericPlatform.h"
#include "Core/Delegates/Event.h"
#include "Core/Input/CEInputManager.h"

class CEWindow;
class CECursor;
class CEConsole;
class CEInputManager;

class CEPlatform {
public:
	CEPlatform() = default;
	virtual ~CEPlatform() = default;

	virtual bool Create();
	virtual bool CreateSystemWindow() = 0;
	virtual bool CreateSystemConsole() = 0;
	virtual bool CreateCursor() = 0;

	virtual bool CreateManagers() {
		if (!CreateInputManager()) {
			return false;
		}

		InputManager->Create();

		return true;
	}

	virtual bool Update() = 0;
	virtual bool Release() = 0;

	virtual void SetCapture(CEWindow* Window) = 0;
	virtual void SetActiveWindow(CEWindow* Window) = 0;
	virtual void SetCursor(CECursor* Cursor) = 0;
	virtual void SetCursorPosition() = 0;
	virtual void SetCallbacks(CEPlatformCallbacks* Callbacks);
	
	virtual CEWindow* GetCapture() = 0;
	virtual CEWindow* GetActiveWindow() = 0;
	virtual CECursor* GetCursor() = 0;
	virtual void GetCursorPosition() = 0;
	virtual CEModifierKeyState GetModifierKeyState() {
		return CEModifierKeyState();
	}
	virtual CEPlatformCallbacks* GetCallbacks();
	virtual CEWindow* GetWindow();
	virtual CEConsole* GetConsole();
	virtual CEInputManager* GetInputManager();

private:
	virtual bool CreateInputManager() = 0;

public:
	inline static CEProjectConfig ProjectConfig;

	inline static CECursor* Cursor;
	inline static CEWindow* Window;
	inline static CEConsole* Console;
	inline static CEInputManager* InputManager;
	inline static CEPlatformCallbacks* Callbacks;
	inline static CEArray<CEEvent<void>> Messages;
	inline static bool bIsMouseTracking;
};
