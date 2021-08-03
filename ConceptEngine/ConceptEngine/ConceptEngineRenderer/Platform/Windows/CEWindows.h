#pragma once
#include "../../Platform/CEPlatform.h"
#include "../../Core/Application/Windows/CEWindowsCursor.h"

class CEWindows final : public CEPlatform {
public:
	CEWindows();
	~CEWindows() override;

	bool Create() override;
	bool CreateSystemWindow() override;
	bool CreateSystemConsole() override;
	bool CreateCursor() override;
	void Update() override;
	bool Release() override;
	void SetCapture(CEWindow* Window) override;
	void SetActiveWindow(CEWindow* Window) override;
	void SetCursor(CECursor* Cursor) override;
	void SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) override;
	CEWindow* GetCapture() override;
	CEWindow* GetActiveWindow() override;
	CECursor* GetCursor() override;
	void GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) override;
	static CEModifierKeyState GetModifierKeyState();

	static void PreInit(HINSTANCE hInstance, LPSTR lpCmdLine, HWND hWnd, int nCmdShow, int width, int height);

	static LPCWSTR GetWindowClassName() {
		return L"WindowClass";
	}

private:
	bool CreateInputManager() override;

	void UpdateRuntime();
	void UpdatePeekMessage();
	void UpdateStoredMessage();

	friend class CEWindowsWindow;
	static LRESULT MessageProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	static void HandleStoredMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	static void StoreMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CEWindow* GetWindow() override;
private:
	inline static CEArray<CEWindowsEvent> Messages;
	inline static CERef<CEWindowsCursor> Cursor;
	inline static CERef<CEWindow> Window;
};
