#pragma once
#include "Platform/CEPlatform.h"

class CELinux : public CEPlatform {
public:
	CELinux();
	virtual ~CELinux() override;
	bool CreateSystemWindow() override;
	bool CreateSystemConsole() override;
	bool CreateCursor() override;
	bool Update() override;
	bool Release() override;
	void SetCapture(CEWindow* Window) override;
	void SetActiveWindow(CEWindow* Window) override;
	void SetCursor(CECursor* Cursor) override;
	void SetCursorPosition() override;
	CEWindow* GetCapture() override;
	CEWindow* GetActiveWindow() override;
	CECursor* GetCursor() override;
	void GetCursorPosition() override;
private:
	bool CreateInputManager() override;
};
