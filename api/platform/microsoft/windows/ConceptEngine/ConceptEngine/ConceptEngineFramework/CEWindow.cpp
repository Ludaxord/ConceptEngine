#include "CEWindow.h"

#include <sstream>
#include <string>

#include "resource.h"

#include "CEWindowMessage.h"
// #include <magic_enum.hpp>

#include "CEHelper.h"
#include "CEOSTools.h"
#include "CETools.h"

CEWindow::CEWindowClass CEWindow::CEWindowClass::wndClass;


CEWindow::CEWindowClass::CEWindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		HandleMsgSetup,
		0,
		0,
		GetInstance(),
		static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)),

		nullptr,
		(HBRUSH)(5 + 1),
		nullptr,
		CETools::ConvertCharArrayToLPCWSTR(GetName()),
		static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0))
	};
	if (!RegisterClassEx(&wc)) {
		MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
	}
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
	pGraphics->OnInit();
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	pGraphics->InitGui();
}

CEWindow::~CEWindow() {
	pGraphics->OnDestroy();
	DestroyWindow(hWnd);
	// CloseWindow(hWnd);
}

void CEWindow::SetTitle(const std::string& title) {
	if (SetWindowText(hWnd, CETools::ConvertCharArrayToLPCWSTR(title.c_str())) == 0) {
		return;
	}
}

void CEWindow::SetGraphicsApi(CEOSTools::CEGraphicsApiTypes graphicsApiType) {
	apiType_ = graphicsApiType;
}

bool CEWindow::RunGraphics() {
	const auto api = CEGraphics::GetGraphicsByApiType(hWnd, apiType_, width, height);
	if (api == nullptr) {
		return false;
	}
	std::unique_ptr<CEGraphics> graphics(api);
	pGraphics = std::move(graphics);
	return true;
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
		const auto api = CEGraphics::GetGraphicsByApiType(hWnd, apiType_, width, height);
		std::unique_ptr<CEGraphics> graphics(api);
		pGraphics = std::move(graphics);
	}
	return *pGraphics;
}

const char* CEWindow::GetName() {
	return "ConceptEngine Editor";
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
	}

	hWnd = CreateWindowEx(
		NULL,
		CETools::ConvertCharArrayToLPCWSTR(CEWindowClass::GetName()),
		CETools::ConvertCharArrayToLPCWSTR(name),
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		CEWindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr) {
	}

	return hWnd;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


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

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE: {
		if (GetWindowType() == CEWindowTypes::main) {
			PostQuitMessage(1);
			return 0;
		}
		GetGraphics().OnDestroy();
		CloseWindow(hWnd);
		break;
	}
	case WM_SIZE:
		if (GetGraphics().IsInitialized()) {
			GetGraphics().ChangeScreenSize(LOWORD(lParam), HIWORD(lParam));
			GetGraphics().OnResize();
		}
		break;

	case WM_KILLFOCUS:
		keyboard.ClearState();
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		//0x4000000 == 30 Source: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
		if (!(lParam & 0x4000000) || keyboard.IsAutoRepeatEnabled()) {
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
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
