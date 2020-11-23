#include "CEWindow.h"

#include <sstream>
#include <string>

#include "resource.h"

#include "CEConverters.h"
#include "CEWindowsMessage.h"

CEWindow::CEWindowClass CEWindow::CEWindowClass::wndClass;

CEWindow::CEWindowClass::CEWindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {
		sizeof(wc),
		CS_OWNDC,
		HandleMsgSetup,
		0,
		0,
		GetInstance(),
		static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0)),
		nullptr,
		nullptr,
		nullptr,
		CEConverters::ConvertCharArrayToLPCWSTR(GetName()),
		static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0))
	};
	RegisterClassEx(&wc);
}

CEWindow::Exception::Exception(int exceptionLine, const char* exceptionFile, HRESULT hresult) noexcept:
	CEException(exceptionLine, exceptionFile), hresult(hresult) {
}

const char* CEWindow::Exception::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] " << GetErrorCode() << std::endl << "[Message] " << GetErrorMessage()
		<< std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* CEWindow::Exception::GetType() const noexcept {
	return "CEWindowException";
}

std::string CEWindow::Exception::TranslateErrorCode(HRESULT hresult) {
	char* pMsgBuffer = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hresult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuffer),
		0,
		nullptr
	);
	if (nMsgLen == 0) {
		return "Unidentified error code";
	}
	std::string errorMessage = pMsgBuffer;
	LocalFree(pMsgBuffer);
	return errorMessage;
}

HRESULT CEWindow::Exception::GetErrorCode() const noexcept {
	return hresult;
}

std::string CEWindow::Exception::GetErrorMessage() const noexcept {
	return TranslateErrorCode(hresult);
}

double CEWindow::CEScreen::CalculateAspectRatio(int horizontal, int vertical) {
	return (double)(horizontal / vertical);
}

int CEWindow::CEScreen::GetRefreshRate() {
	DEVMODE mode;
	memset(&mode, 0, sizeof(mode));
	mode.dmSize = sizeof(mode);

	if (!EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode))
		return 60;
	return mode.dmDisplayFrequency;
}

CEWindow::CEScreen CEWindow::CEScreen::GetScreenInfo(CEScreenTypes type) {
	int horizontal = 0;
	int vertical = 0;
	switch (type) {
	case CEScreenTypes::primary: {
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}
	break;
	case CEScreenTypes::allCombined: {
	}
	break;
	case CEScreenTypes::specific: {
	}
	break;
	case CEScreenTypes::workingArea: {
	}
	break;
	}
	auto screen = CEScreen();
	screen.vertical = vertical;
	screen.horizontal = horizontal;
	screen.aspectRatio = CalculateAspectRatio(horizontal, vertical);
	screen.refreshRate = GetRefreshRate();
	return screen;
}

const char* CEWindow::CEWindowClass::GetName() noexcept {
	return wndClassName;
}

CEWindow::CEWindowTypes CEWindow::GetWindowType() noexcept {
	return wndType;
}

void CEWindow::SetWindowType(CEWindowTypes windowType) noexcept {
	wndType = windowType;
}

HINSTANCE CEWindow::CEWindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}


CEWindow::CEWindowClass::~CEWindowClass() {
	UnregisterClass(CEConverters::ConvertCharArrayToLPCWSTR(wndClassName), GetInstance());
}

CEWindow::CEWindow(int width, int height, const char* name, CEWindowTypes windowTypes): width(width), height(height) {
	//Start options
	RECT winLoc;
	winLoc.left = 100;
	winLoc.right = width + winLoc.left;
	winLoc.top = 100;
	winLoc.bottom = height + winLoc.top;
	if (AdjustWindowRect(&winLoc, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0) {
		throw CEWIN_LAST_EXCEPTION();
	}
	hWnd = CreateWindow(
		CEConverters::ConvertCharArrayToLPCWSTR(CEWindowClass::GetName()),
		CEConverters::ConvertCharArrayToLPCWSTR(name),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winLoc.right - winLoc.left,
		winLoc.bottom - winLoc.top,
		nullptr,
		nullptr,
		CEWindowClass::GetInstance(),
		this //passing CEWindow API function to get in HandleMsgSetup
	);
	SetWindowType(windowTypes);
	if (hWnd == nullptr) {
		throw CEWIN_LAST_EXCEPTION();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	pGraphics = std::make_unique<CEGraphics>(hWnd);
}

CEWindow::~CEWindow() {
	DestroyWindow(hWnd);
}

void CEWindow::SetTitle(const std::string& title) {
	if (SetWindowText(hWnd, CEConverters::ConvertCharArrayToLPCWSTR(title.c_str())) == 0) {
		throw CEWIN_LAST_EXCEPTION();
	}
}

CEWindow::CEScreen CEWindow::GetScreenInfo() {
	return CEScreen::GetScreenInfo();
}

std::optional<int> CEWindow::ProcessMessages() {
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

CEGraphics& CEWindow::GetGraphics() {
	return *pGraphics;
}

LRESULT WINAPI CEWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		CEWindow* const pWnd = static_cast<CEWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)); // creating window

		//passing pointer to HandleMsgThunk to make it main function to call during lifecycle of window (if user press key or close window) 
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CEWindow::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI CEWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	CEWindow* const pWnd = reinterpret_cast<CEWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CEWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {

	//print Windows Message 
	static CEWindowsMessage wm;
	if (GetWindowType() == debug) {
		OutputDebugString(CEConverters::ConvertCharArrayToLPCWSTR(wm(msg, lParam, wParam).c_str()));
	}

	std::ostringstream cen;
	cen << "Concept Engine Window, Type: " << GetWindowType() << "\n";
	OutputDebugString(CEConverters::ConvertCharArrayToLPCWSTR(cen.str().c_str()));

	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE: {
		if (GetWindowType() == main) {
			PostQuitMessage(1);
			return 0;
		}
		CloseWindow(hWnd);
		break;
	}
	case WM_KILLFOCUS:
		keyboard.ClearState();
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		//0x4000000 == 30 Source: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
		if (!(lParam & 0x4000000) || keyboard.IsAutoRepeatEnabled()) {
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
	case WM_CHAR:
		keyboard.OnChar(static_cast<char>(wParam));
		break;
	case WM_MOUSEMOVE: {
		const auto pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsMouseInWindow()) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else {
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}

		break;
	}
	case WM_LBUTTONDOWN: {
		const auto pt = MAKEPOINTS(lParam);
		mouse.OnLeftKeyPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN: {
		const auto pt = MAKEPOINTS(lParam);
		mouse.OnRightKeyPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP: {
		const auto pt = MAKEPOINTS(lParam);
		mouse.OnLeftKeyReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP: {
		const auto pt = MAKEPOINTS(lParam);
		mouse.OnRightKeyReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL: {
		const auto pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
