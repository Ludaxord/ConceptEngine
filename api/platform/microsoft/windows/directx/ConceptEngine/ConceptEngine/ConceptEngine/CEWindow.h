#pragma once
#include <optional>

#include "CEException.h"
#include "CEWin.h"
#include "CEKeyboard.h"
#include "CEMouse.h"

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

	//TODO: create separate class for screen
	enum class CEScreenTypes {
		primary,
		allCombined,
		workingArea,
		specific
	};

	//TODO: create separate class for screen
	struct CEScreen {
		int horizontal = 0;
		int vertical = 0;
		double aspectRatio = 0;
		double refreshRate = 0;
		//TODO: fix aspect ratio double
		static double CalculateAspectRatio(int horizontal = 0, int vertical = 0);
		static int GetRefreshRate();
		static CEScreen GetScreenInfo(CEScreenTypes type = CEScreenTypes::primary);
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
		//TODO: check constexpr meaning
		static constexpr const char* wndClassName = "Concept Engine";
		static CEWindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	CEWindow(int width, int height, const char* name, CEWindowTypes windowType = main);
	~CEWindow();
	CEWindow(const CEWindow&) = delete;
	CEWindow& operator =(const CEWindow&) = delete;
	void SetTitle(const std::string& title);
	CEScreen GetScreenInfo();
	static std::optional<int> ProcessMessages();
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
private:
	int width;
	int height;
	HWND hWnd;
};

#define CEWIN_EXCEPTION(hresult) CEWindow::Exception(__LINE__, __FILE__, hresult)
#define CEWIN_LAST_EXCEPTION() CEWindow::Exception(__LINE__, __FILE__, GetLastError())
