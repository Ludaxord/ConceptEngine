#include "CEWindowsWindow.h"

#include "../../../../Memory/CEMemory.h"
#include "../CEWindows.h"
#include "../../../../ConceptEngine.h"

#include "../../../Log/CELog.h"

using namespace ConceptEngine::Core::Platform::Windows::Window;


CEWindowsWindow::CEWindowsWindow(): CEWindow() {
}

CEWindowsWindow::~CEWindowsWindow() {
}

bool CEWindowsWindow::Create() {
	if (!RegisterWindowClass()) {
		return false;
	}
	return true;
}

bool CEWindowsWindow::RegisterWindowClass() {
	WNDCLASS windowClass;
	Memory::CEMemory::Memzero(&windowClass);

	windowClass.hInstance = CEWindows::Instance;
	windowClass.lpszClassName = ConceptEngine::GetName().c_str();
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
}

void CEWindowsWindow::Maximize() {
}

void CEWindowsWindow::Close() {
}

void CEWindowsWindow::Restore() {
}

void CEWindowsWindow::ToggleFullscreen() {
}

bool CEWindowsWindow::IsValid() const {
	return IsWindow(CEWindows::HWnd) == TRUE;
}

bool CEWindowsWindow::IsActiveWindow() const {
	return true;
}

void CEWindowsWindow::SetTitle(const std::string& title) {
}

void CEWindowsWindow::GetTitle(const std::string& outTitle) {
}

void CEWindowsWindow::SetWindowSize(const Generic::Window::CEWindowSize& shape, bool move) {
}

void CEWindowsWindow::GetWindowSize(Generic::Window::CEWindowSize& outShape) {
}

uint32 CEWindowsWindow::GetWidth() const {
	return 0;
}

uint32 CEWindowsWindow::GetHeight() const {
	return 0;
}
