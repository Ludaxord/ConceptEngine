#pragma once
#include "../Config/CEGlobalConfigs.h"
#include "../Core/Application/Generic/CEGenericPlatform.h"
#include "../Core/Delegates/Event.h"
#include "../Core/Input/CEInputManager.h"
#include "../Core/Application/Windows/CEWindowsPlatform.h"

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

	virtual void Update() = 0;
	virtual bool Release() = 0;

	virtual void SetCapture(CEWindow* Window) = 0;
	virtual void SetActiveWindow(CEWindow* Window) = 0;
	virtual void SetCursor(CECursor* Cursor) = 0;
	virtual void SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) = 0;
	virtual void SetCallbacks(CEPlatformCallbacks* Callbacks);
	
	virtual CEWindow* GetCapture() = 0;
	virtual CEWindow* GetActiveWindow() = 0;
	virtual CECursor* GetCursor() = 0;
	virtual void GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) = 0;
	virtual CERef<CEWindow> GetWindow() = 0;

	static CEModifierKeyState GetModifierKeyState() {
		return CEModifierKeyState();
	}
	static CEPlatformCallbacks* GetCallbacks();
	static CEConsole* GetConsole();
	static CEInputManager* GetInputManager();

private:
	virtual bool CreateInputManager() = 0;

public:
	inline static CEProjectConfig ProjectConfig;
	
	inline static CEConsole* Console;
	inline static CEInputManager* InputManager;
	inline static CEPlatformCallbacks* Callbacks;
	inline static bool bIsMouseTracking;
};
