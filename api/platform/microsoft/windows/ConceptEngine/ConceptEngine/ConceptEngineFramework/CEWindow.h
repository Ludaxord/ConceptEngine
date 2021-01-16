#pragma once
#include <optional>

#include "CELib.h"
#include "CEKeyboard.h"
#include "CEMouse.h"
#include "CEGraphics.h"
#include <memory>

#include "imgui_impl_win32.h"

class CEWindow {

public:

	enum class CEWindowTypes {
		main,
		debug,
		additional,
		tools
	};

	enum class CEScreenTypes {
		primary,
		allCombined,
		workingArea,
		specific
	};

	struct CEScreen {
		int horizontal = 0;
		int vertical = 0;
		double aspectRatio = 0;
		double refreshRate = 0;
		//TODO: fix aspect ratio double
		static double CalculateAspectRatio(int horizontal = 0, int vertical = 0);
		static int GetRefreshRate();
		static CEScreen GetScreenInfo(CEWindow::CEScreenTypes type = CEScreenTypes::primary);
	};

private:
	class CEWindowClass {
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		CEWindowClass() noexcept;
		~CEWindowClass();
		CEWindowClass(const CEWindowClass&) = delete;
		static constexpr const char* wndClassName = "Concept Engine";
		static CEWindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	CEWindow(int width, int height, const char* name, CEWindowTypes windowType = CEWindowTypes::main);
	CEWindow(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);
	~CEWindow();
	CEWindow(const CEWindow&) = delete;
	CEWindow& operator =(const CEWindow&) = delete;
	
	void SetTitle(const std::string& title);
	void SetGraphicsApi(CEOSTools::CEGraphicsApiTypes graphicsApiType);
	bool RunGraphics();
	CEScreen GetScreenInfo();
	static std::optional<int> ProcessMessages() noexcept;
	CEGraphics& GetGraphics();
	static const char* GetName();

	HWND CreateMainWindow(const char* name);
	void RegisterWindowClass();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	CEWindowTypes wndType;
protected:
	CEWindowTypes GetWindowType() noexcept;
	void SetWindowType(CEWindowTypes windowType) noexcept;
public:
	CEKeyboard keyboard;
	CEMouse mouse;
	static const char* windowName_;

private:
	int width;
	int height;

	HWND hWnd;
	CEOSTools::CEGraphicsApiTypes apiType_;
	std::unique_ptr<CEGraphics> pGraphics;

};
