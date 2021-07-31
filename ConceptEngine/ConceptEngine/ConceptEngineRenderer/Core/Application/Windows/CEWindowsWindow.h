#pragma once
#include "Windows.h"

#include "../../../Core/Application/Generic/CEWindow.h"

static void GetScreenResolution(int& width, int& height) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	width = desktop.right;
	height = desktop.bottom;
}

static const uint32 DefaultStyle =
	WindowStyleFlag_Titled |
	WindowStyleFlag_Closable |
	WindowStyleFlag_Minimizable |
	WindowStyleFlag_Maximizable |
	WindowStyleFlag_Resizeable;

class CEWindowsWindow;

class CEWindowsWindowSize : public CEWindowSize {
public:
	static void Create(int width, int height, int x = 0, int y = 0) {
		WndSize = new CEWindowsWindowSize(width, height, x, y);
		WndSize->CreateSize();
		SetResolution(width, height);
		SetPosition(x, y);
	}

	void CreateSize(int width, int height) override {
		GetScreenResolution(ScreenWidth, ScreenHeight);
	};

	friend class CEWindowsWindow;

	CEWindowsWindowSize(int width, int height, int x, int y): CEWindowSize(width, height, x, y) {

	}

	CEWindowsWindowSize(): CEWindowSize() {

	}
};

class CEWindowsWindow : public CEWindow {
public:
	CEWindowsWindow();
	~CEWindowsWindow();

	bool Create(const std::wstring& Title, uint32 Width, uint32 Height, CEWindowStyle Style);

	virtual void Show(bool Maximized) override final;
	virtual void Close() override final;
	virtual void Minimize() override final;
	virtual void Maximize() override final;
	virtual void Restore() override final;
	virtual void ToggleFullscreen() override final;

	virtual bool IsValid() const override final;
	virtual bool IsActiveWindow() const override final;

	virtual void SetTitle(const std::string& Title) override final;
	virtual void GetTitle(std::string& OutTitle) override final;

	virtual void SetWindowShape(const CEWindowShape& Shape, bool Move) override final;
	virtual void GetWindowShape(CEWindowShape& OutWindowShape) const override final;

	virtual uint32 GetWidth() const override final;
	virtual uint32 GetHeight() const override final;

	virtual void* GetNativeHandle() const override final {
		return reinterpret_cast<void*>(Window);
	}

	HWND GetHandle() const {
		return Window;
	}

private:
	HWND Window;

	DWORD Style;
	DWORD StyleEx;

	bool IsFullscreen;

	WINDOWPLACEMENT StoredPlacement;
};
