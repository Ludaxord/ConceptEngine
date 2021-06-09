#include "CEWindowsWindow.h"

#include "../../../../Memory/CEMemory.h"
#include "../CEWindows.h"
#include "../../../../CEEngine.h"

#include "../../../Log/CELog.h"

#include "../../../Debug/CEDebug.h"

using namespace ConceptEngine::Core::Platform::Windows::Window;


CEWindowsWindow::CEWindowsWindow(): CEWindow(), Style(0), StyleEx(0), IsFullscreen(false) {
}

CEWindowsWindow::~CEWindowsWindow() {
	if (IsValid()) {
		DestroyWindow(CEWindows::HWnd);
	}
}

bool CEWindowsWindow::Create() {
	if (!RegisterWindowClass()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEWindowsWindow::RegisterWindowClass() {
	WNDCLASS windowClass;
	Memory::CEMemory::Memzero(&windowClass);

	windowClass.hInstance = CEWindows::Instance;
	windowClass.lpszClassName = CEEngine::GetName().c_str();
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpfnWndProc = CEWindows::MessageProc;

	auto classAtm = RegisterClass(&windowClass);
	if (classAtm == 0) {
		CE_LOG_ERROR("[CEWindows]: Failed to register CEWindowsWindow Class\n");
		return false;
	}

	return true;
}

bool CEWindowsWindow::Create(const std::string& title, uint32 width, uint32 height,
                             Generic::Window::CEWindowStyle style) {
	if (!Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	DWORD dwStyle = 0;
	if (style.Style != 0) {
		if (style.IsTitled()) {
			dwStyle |= WS_CAPTION;
		}
		if (style.IsClosable()) {
			dwStyle |= WS_SYSMENU;
		}
		if (style.IsMinimizable()) {
			dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
		}
		if (style.IsMaximizable()) {
			dwStyle |= WS_SYSMENU | WS_MAXIMIZEBOX;
		}
		if (style.IsResizable()) {
			dwStyle |= WS_THICKFRAME;
		}
	}
	else {
		dwStyle = WS_POPUP;
	}

	RECT clientRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
	AdjustWindowRect(&clientRect, dwStyle, FALSE);

	INT nWidth = clientRect.right - clientRect.left;
	INT nHeight = clientRect.bottom - clientRect.top;

	HINSTANCE Instance = CEWindows::Instance;
	auto windowClassName = CEWindows::GetWindowClassName();
	CEWindows::HWnd = CreateWindowEx(0,
	                                 windowClassName,
	                                 std::wstring(title.begin(), title.end()).c_str(),
	                                 dwStyle,
	                                 CW_USEDEFAULT,
	                                 CW_USEDEFAULT,
	                                 nWidth,
	                                 nHeight,
	                                 NULL,
	                                 NULL,
	                                 Instance,
	                                 NULL
	);

	if (CEWindows::HWnd == NULL) {
		CE_LOG_ERROR("[CEWindowsWindow] Failed to create Window \n");
		return false;
	}
	else {
		if (dwStyle & WS_SYSMENU) {
			if (!(style.IsClosable())) {
				EnableMenuItem(GetSystemMenu(CEWindows::HWnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
		}

		Style = dwStyle;
		WindowStyle = style;

		SetLastError(0);

		LONG_PTR result = SetWindowLongPtrA(CEWindows::HWnd, GWLP_USERDATA, (LONG_PTR)this);
		DWORD lastError = GetLastError();
		if (result == 0 && lastError != 0) {
			CE_LOG_ERROR("[CEWindowsWindow] Failed to setup window-data\n");
			return false;
		}

		UpdateWindow(CEWindows::HWnd);
		return true;
	}
}

void CEWindowsWindow::Show(bool maximized) {
	Assert(CEWindows::HWnd != 0);
	if (IsValid()) {
		maximized ? ShowWindow(CEWindows::HWnd, SW_NORMAL) : ShowWindow(CEWindows::HWnd, SW_SHOWMAXIMIZED);
	}
}

void CEWindowsWindow::Minimize() {
	Assert(CEWindows::HWnd != 0);
	if (WindowStyle.IsMinimizable()) {
		if (IsValid()) {
			ShowWindow(CEWindows::HWnd, SW_MINIMIZE);
		}
	}
}

void CEWindowsWindow::Maximize() {
	Assert(CEWindows::HWnd != 0);
	if (WindowStyle.IsMaximizable()) {
		if (IsValid()) {
			ShowWindow(CEWindows::HWnd, SW_MAXIMIZE);
		}
	}
}

void CEWindowsWindow::Close() {
	Assert(CEWindows::HWnd != 0);
	if (WindowStyle.IsClosable()) {
		if (IsValid()) {
			CloseWindow(CEWindows::HWnd);
		}
	}
}

void CEWindowsWindow::Restore() {
	Assert(CEWindows::HWnd != 0);
	if (IsValid()) {
		bool result = ::IsIconic(CEWindows::HWnd);
		if (result) {
			::ShowWindow(CEWindows::HWnd, SW_RESTORE);
		}
	}
}

void CEWindowsWindow::ToggleFullscreen() {
	Assert(CEWindows::HWnd != 0);
	if (IsValid()) {
		if (!IsFullscreen) {
			IsFullscreen = true;

			::GetWindowPlacement(CEWindows::HWnd, &StoredPlacement);
			if (Style == 0) {
				Style = ::GetWindowLong(CEWindows::HWnd, GWL_STYLE);
			}
			if (StyleEx == 0) {
				Style = ::GetWindowLong(CEWindows::HWnd, GWL_EXSTYLE);
			}

			LONG newStyle = Style;
			newStyle &= ~WS_BORDER;
			newStyle &= ~WS_DLGFRAME;
			newStyle &= ~WS_THICKFRAME;

			LONG newStyleEx = StyleEx;
			newStyleEx &= ~WS_EX_WINDOWEDGE;

			SetWindowLong(CEWindows::HWnd, GWL_STYLE, newStyle | WS_POPUP);
			SetWindowLong(CEWindows::HWnd, GWL_EXSTYLE, newStyleEx | WS_EX_TOPMOST);
			ShowWindow(CEWindows::HWnd, SW_SHOWMAXIMIZED);
		}
		else {
			IsFullscreen = false;
			SetWindowLong(CEWindows::HWnd, GWL_STYLE, Style);
			SetWindowLong(CEWindows::HWnd, GWL_EXSTYLE, StyleEx);
			ShowWindow(CEWindows::HWnd, SW_SHOWNORMAL);
			SetWindowPlacement(CEWindows::HWnd, &StoredPlacement);
		}
	}
}

bool CEWindowsWindow::IsValid() const {
	return IsWindow(CEWindows::HWnd) == TRUE;
}

bool CEWindowsWindow::IsActiveWindow() const {
	HWND hActive = GetForegroundWindow();
	return (hActive == CEWindows::HWnd);
}

void CEWindowsWindow::SetTitle(const std::string& title) {
	Assert(CEWindows::HWnd != 0);
	if (WindowStyle.IsTitled()) {
		if (IsValid()) {
			SetWindowTextA(CEWindows::HWnd, title.c_str());
		}
	}
}

void CEWindowsWindow::GetTitle(std::string& outTitle) {
	if (IsValid()) {
		int32 size = GetWindowTextLengthA(CEWindows::HWnd);
		outTitle.resize(size);

		GetWindowTextA(CEWindows::HWnd, outTitle.data(), size);
	}
}

void CEWindowsWindow::SetWindowSize(const Generic::Window::CEWindowSize& shape, bool move) {
	Assert(CEWindows::HWnd);
	if (IsValid()) {
		UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
		if (!move) {
			flags |= SWP_NOMOVE;
		}

		auto windowsShape = reinterpret_cast<const CEWindowsWindowSize&>(shape);

		SetWindowPos(
			CEWindows::HWnd,
			NULL,
			windowsShape.WindowPosition.x,
			windowsShape.WindowPosition.y,
			windowsShape.Width,
			windowsShape.Height,
			flags
		);
	}
}

void CEWindowsWindow::GetWindowSize(Generic::Window::CEWindowSize& outShape) {
	Assert(CEWindows::HWnd != 0);
	if (IsValid()) {
		int32 x = 0;
		int32 y = 0;
		uint32 width = 0;
		uint32 height = 0;

		RECT Rect = {};
		if (GetWindowRect(CEWindows::HWnd, &Rect) != 0) {
			x = static_cast<int32>(Rect.left);
			y = static_cast<int32>(Rect.top);
		}

		if (GetClientRect(CEWindows::HWnd, &Rect) != 0) {
			width = static_cast<uint32>(Rect.right - Rect.left);
			height = static_cast<uint32>(Rect.bottom - Rect.top);
		}

		outShape = CEWindowsWindowSize(width, height, x, y);
	}
}

uint32 CEWindowsWindow::GetWidth() const {
	if (IsValid()) {
		RECT rect = {};
		if (GetClientRect(CEWindows::HWnd, &rect) != 0) {
			const uint32 width = static_cast<uint32>(rect.right - rect.left);
			return width;
		}
	}
	return 0;
}

uint32 CEWindowsWindow::GetHeight() const {
	if (IsValid()) {
		RECT rect = {};
		if (GetClientRect(CEWindows::HWnd, &rect) != 0) {
			const uint32 height = static_cast<uint32>(rect.bottom - rect.top);
			return height;
		}
	}
	return 0;
}

HWND CEWindowsWindow::GetHandle() const {
	return CEWindows::HWnd;
}
