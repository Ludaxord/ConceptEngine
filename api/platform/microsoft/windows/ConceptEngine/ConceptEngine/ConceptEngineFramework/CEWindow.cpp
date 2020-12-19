#include "CEWindow.h"

#include <sstream>
#include <string>

#include "resource.h"

#include "CEWindowMessage.h"
#include <magic_enum.hpp>


#include "CEHelper.h"
#include "CEOSTools.h"
#include "CETools.h"

CEWindow::CEWindowClass CEWindow::CEWindowClass::wndClass;

CEWindow::CEWindowClass::CEWindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		// CS_OWNDC,
		CS_HREDRAW | CS_VREDRAW,
		HandleMsgSetup,
		0,
		0,
		GetInstance(),
		// static_cast<HICON>(LoadImage(hInst,
		//                              MAKEINTRESOURCE(IDI_ICON2),
		//                              IMAGE_ICON, 32, 32, 0)),
		::LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2)),
		nullptr,
		(HBRUSH)(5 + 1),
		nullptr,
		CETools::ConvertCharArrayToLPCWSTR(GetName()),
		// static_cast<HICON>(LoadImage(hInst,
		//                              MAKEINTRESOURCE(IDI_ICON2),
		//                              IMAGE_ICON, 16, 16, 0))
		::LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2))
	};
	if (!RegisterClassEx(&wc)) {
		MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
	}
}

std::string CEWindow::Exception::TranslateErrorCode(HRESULT hresult) noexcept {
	char* pMsgBuffer = nullptr;
	const DWORD nMsgLen = FormatMessage(
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

CEWindow::HResultException::HResultException(int line, const char* file, HRESULT hresult) noexcept :
	Exception(line, file),
	hresult(hresult) {
}

HRESULT CEWindow::HResultException::GetErrorCode() const noexcept {
	return hresult;
}

const char* CEWindow::HResultException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec <<
		" (" << (unsigned long)GetErrorCode() << ")" << std::endl << "[Description] " << GetErrorDescription() <<
		std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* CEWindow::HResultException::GetType() const noexcept {
	return "Concept Engine Window Exception";
}

std::string CEWindow::HResultException::GetErrorDescription() const noexcept {
	return TranslateErrorCode(hresult);
}

const char* CEWindow::GraphicsException::GetType() const noexcept {
	return "Concept Engine Window Exception [No Graphics]";
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
	UnregisterClass(CETools::ConvertCharArrayToLPCWSTR(wndClassName), GetInstance());
}

CEWindow::CEWindow(int width, int height, const char* name, CEWindowTypes windowTypes) : width(width), height(height) {
	CreateMainWindow(name);
	SetWindowType(windowTypes);

}

CEWindow::CEWindow(int width, int height, const char* name,
                   CEOSTools::CEGraphicsApiTypes graphicsApiType): CEWindow(width, height, name) {
	SetGraphicsApi(graphicsApiType);
	RunGraphics();
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

CEWindow::~CEWindow() {
	DestroyWindow(hWnd);
	// CloseWindow(hWnd);
}

void CEWindow::SetTitle(const std::string& title) {
	if (SetWindowText(hWnd, CETools::ConvertCharArrayToLPCWSTR(title.c_str())) == 0) {
		throw CEWIN_LAST_EXCEPTION();
	}
}

void CEWindow::SetGraphicsApi(CEOSTools::CEGraphicsApiTypes graphicsApiType) {
	apiType_ = graphicsApiType;
}

void CEWindow::RunGraphics() {
	const auto api = CEGraphics::GetGraphicsByApiType(hWnd, apiType_, width, height);
	std::unique_ptr<CEGraphics> graphics(api);
	pGraphics = std::move(graphics);
}

CEWindow::CEScreen CEWindow::GetScreenInfo() {
	return CEScreen::GetScreenInfo();
}

std::optional<int> CEWindow::ProcessMessages() noexcept {
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
	if (!pGraphics) {
		throw CEWIN_NOGFX_EXCEPTION();
	}
	return *pGraphics;
}


void CEWindow::RegisterWindowClass() {
}

HWND CEWindow::CreateMainWindow(const char* name) {

	//Start options
	int screenWidth = 0;
	int screenHeight = 0;
	GetDesktopResolution(screenWidth, screenHeight);
	RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

	if (AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE) == 0) {
		throw CEWIN_LAST_EXCEPTION();
	}

	std::wstringstream wss;
	wss << "screenWidth: " << screenWidth << std::endl;
	wss << "screenHeight: " << screenHeight << std::endl;
	wss << "windowWidth: " << windowWidth << std::endl;
	wss << "windowHeight: " << windowHeight << std::endl;
	wss << "windowX: " << windowX << std::endl;
	wss << "windowY: " << windowY << std::endl;

	OutputDebugStringW(wss.str().c_str());

	hWnd = CreateWindowEx(
		NULL,
		CETools::ConvertCharArrayToLPCWSTR(CEWindowClass::GetName()),
		CETools::ConvertCharArrayToLPCWSTR(name),
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		CEWindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr) {
		throw CEWIN_LAST_EXCEPTION();
	}

	return hWnd;
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
	static CEWindowMessage wm;
	if (GetWindowType() == CEWindowTypes::debug) {
		OutputDebugString(CETools::ConvertCharArrayToLPCWSTR(wm(msg, lParam, wParam).c_str()));
	}

	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE: {
		if (GetWindowType() == CEWindowTypes::main) {
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
	case WM_SYSKEYDOWN: {
		//0x4000000 == 30 Source: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
		if (!(lParam & 0x4000000) || keyboard.IsAutoRepeatEnabled()) {
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		const bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		switch (wParam) {
		case 'V':
			pGraphics->ChangeVSyncState();
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			if (alt) {
			case VK_F11:
				auto fullscreen = pGraphics->GetFullScreenState();
				pGraphics->SetFullscreen(!fullscreen);
			}
			break;
		}
	}
	break;
	case WM_DESTROY:
		::PostQuitMessage(0);
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
