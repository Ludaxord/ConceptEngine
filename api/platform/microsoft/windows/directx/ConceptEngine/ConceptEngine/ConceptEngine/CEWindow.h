#pragma once
#include "CEException.h"
#include "CEWin.h"

class CEWindow {

public:

	class Exception : public CEException {
	public:
		Exception(int exceptionLine, const char* exceptionFile, HRESULT hresult) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hresult);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorMessage() const noexcept;
	private:
		HRESULT hresult;
	};

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
		//TODO: change from static and move to CEWindow
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
		//TODO: move to CEWindow
		CEWindowTypes wndType;
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

#define CEWIN_EXCEPTION(hresult) CEWindow::Exception(__LINE__, __FILE__, hresult)
#define CEWIN_LAST_EXCEPTION() CEWindow::Exception(__LINE__, __FILE__, GetLastError())
