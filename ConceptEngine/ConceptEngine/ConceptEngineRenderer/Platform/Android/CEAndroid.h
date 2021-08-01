#pragma once
#include "Platform/CEPlatform.h"

class CEAndroid : public CEPlatform {
public:
	CEAndroid();
	virtual ~CEAndroid() override;

	bool CreateSystemWindow() override;
	bool CreateSystemConsole() override;
	bool CreateCursor() override;
	void Update() override;
	bool Release() override;
	void SetCapture(CEWindow* Window) override;
	void SetActiveWindow(CEWindow* Window) override;
	void SetCursor(CECursor* Cursor) override;
	CEWindow* GetCapture() override;
	CEWindow* GetActiveWindow() override;
	CECursor* GetCursor() override;
private:
	bool CreateInputManager() override;
public:
	void SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) override;
	void GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) override;
	CEWindow* GetWindow() override;
};
