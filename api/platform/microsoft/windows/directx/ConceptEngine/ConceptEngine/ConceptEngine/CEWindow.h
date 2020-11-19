#pragma once
#include "CEWin.h"

class CEWindow {

public:
	enum CEWindowTypes {
		main,
		debug,
		additional,
		tools
	};
	
private:
	class CEWindowClass {
	public:
		static const char* GetName() noexcept;
		static CEWindowTypes GetWindowType() noexcept;
		static void SetWindowType(CEWindowTypes windowType) noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		CEWindowClass() noexcept;
		~CEWindowClass();
		CEWindowClass(const CEWindowClass&) = delete;
		//TODO: check constexpr meaning
		static constexpr const char* wndClassName = "Concept Engine";
		static CEWindowClass wndClass;
		static CEWindowTypes wndType;
		HINSTANCE hInst;
	};

public:
	CEWindow(int width, int height, const char* name, CEWindowTypes windowType = main);
	~CEWindow();
	CEWindow(const CEWindow&) = delete;
	CEWindow& operator =(const CEWindow&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	int width;
	int height;
	HWND hWnd;
};
