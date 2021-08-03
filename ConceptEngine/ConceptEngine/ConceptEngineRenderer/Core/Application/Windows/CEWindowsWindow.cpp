#include "CEWindowsWindow.h"
#include "CEWindowsPlatform.h"
#include "Platform/Windows/CEWindows.h"

// CEWindow* CEWindow::Create(const std::wstring& InTitle, uint32 InWidth, uint32 InHeight, CEWindowStyle InStyle) {
// 	CERef<CEWindowsWindow> NewWindow = DBG_NEW CEWindowsWindow();
// 	if (!NewWindow->Create(InTitle, InWidth, InHeight, InStyle)) {
// 		return nullptr;
// 	}
//
// 	return NewWindow.ReleaseOwnership();
// }

CEWindowsWindow::CEWindowsWindow()
	: CEWindow()
	  , Window(0)
	  , Style(0)
	  , StyleEx(0)
	  , IsFullscreen(false) {
}

CEWindowsWindow::~CEWindowsWindow() {
	if (IsValid()) {
		DestroyWindow(Window);
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
	// WNDCLASS windowClass;
	// Memory::CEMemory::Memzero(&windowClass);
	//
	// windowClass.hInstance = CEWindows::Instance;
	// windowClass.lpszClassName = CEEngine::GetName().c_str();
	// windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	// windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// windowClass.lpfnWndProc = CEWindows::MessageProc;
	//
	// auto classAtm = RegisterClass(&windowClass);
	// if (classAtm == 0) {
	// 	CE_LOG_ERROR("[CEWindows]: Failed to register CEWindowsWindow Class\n");
	// 	return false;
	// }

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		CE_LOG_ERROR("[CEWindowsWindow]: Failed to CoInitialize\n")
		return false;
	}

	WNDCLASSEXW wndClass = {0};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &CEWindows::MessageProc;
	wndClass.hInstance = CEPlatform::ProjectConfig.PInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = static_cast<HICON>(LoadImage(CEPlatform::ProjectConfig.PInstance, nullptr, IMAGE_ICON, 32, 32, 0));
    wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = CEWindows::GetWindowClassName();
	wndClass.hIconSm = static_cast<HICON>(LoadImage(CEPlatform::ProjectConfig.PInstance, nullptr, IMAGE_ICON, 16, 16, 0));

	if (!RegisterClassExW(&wndClass)) {
		CE_LOG_ERROR("[CEWindowsWindow]: Unable to register window class.\n")
		return false;
	}

	return true;
}

bool CEWindowsWindow::Create(const std::wstring& InTitle, uint32 InWidth, uint32 InHeight, CEWindowStyle InStyle) {
	if (!Create()) {
		CE_LOG_ERROR("Failed to Register Window class...")
		CEDebug::DebugBreak();
		return false;
	}

	// Determine the window style for WinAPI
	DWORD dwStyle = 0;
	if (InStyle.Style != 0) {
		dwStyle = WS_OVERLAPPED;
		if (InStyle.IsTitled()) {
			dwStyle |= WS_CAPTION;
		}
		if (InStyle.IsClosable()) {
			dwStyle |= WS_SYSMENU;
		}
		if (InStyle.IsMinimizable()) {
			dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
		}
		if (InStyle.IsMaximizable()) {
			dwStyle |= WS_SYSMENU | WS_MAXIMIZEBOX;
		}
		if (InStyle.IsResizeable()) {
			dwStyle |= WS_THICKFRAME;
		}
	}
	else {
		dwStyle = WS_POPUP;
	}

	// Calculate real window size, since the width and height describe the clientarea
	RECT ClientRect = {0, 0, static_cast<LONG>(InWidth), static_cast<LONG>(InHeight)};
	AdjustWindowRect(&ClientRect, dwStyle, FALSE);

	// INT nWidth = ClientRect.right - ClientRect.left;
	// INT nHeight = ClientRect.bottom - ClientRect.top;
	//
	// HINSTANCE Instance = CEWindowsPlatform::GetInstance();
	// LPCWSTR WindowClassName = CEWindowsPlatform::GetWindowClassName();
	// Window = CreateWindowEx(0, WindowClassName, InTitle.c_str(), dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, nWidth, nHeight,
	//                         NULL, NULL, Instance, NULL);


	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	int windowX = std::max<int>(0, (screenWidth - (int)InWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - (int)InHeight) / 2);

	Window = ::CreateWindowExW(NULL,
	                           CEWindows::GetWindowClassName(),
	                           std::wstring(InTitle.begin(), InTitle.end()).c_str(),
	                           dwStyle,
	                           windowX,
	                           windowY,
	                           InWidth,
	                           InHeight,
	                           NULL,
	                           NULL,
	                           CEPlatform::ProjectConfig.PInstance,
	                           NULL);

	if (Window == NULL) {
		CE_LOG_ERROR("[WindowsWindow]: FAILED to create window\n");
		return false;
	}
	else {
		// If the window has a sysmenu we check if the closebutton should be active
		if (dwStyle & WS_SYSMENU) {
			if (!(InStyle.IsClosable())) {
				EnableMenuItem(GetSystemMenu(Window, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
		}

		// Save style for later
		Style = dwStyle;
		WndStyle = InStyle;

		// Set this to userdata
		SetLastError(0);

		LONG_PTR Result = SetWindowLongPtrA(Window, GWLP_USERDATA, (LONG_PTR)this);
		DWORD LastError = GetLastError();
		if (Result == 0 && LastError != 0) {
			CE_LOG_ERROR("[WindowsWindow]: FAILED to Setup window-data\n");
			return false;
		}

		UpdateWindow(Window);
		return true;
	}
}

void CEWindowsWindow::Show(bool Maximized) {
	Assert(Window != 0);

	if (IsValid()) {
		if (Maximized) {
			ShowWindow(Window, SW_NORMAL);
		}
		else {
			ShowWindow(Window, SW_SHOWMAXIMIZED);
		}
	}
}

void CEWindowsWindow::Close() {
	Assert(Window != 0);

	if (IsValid()) {
		if (WndStyle.IsClosable()) {
			CloseWindow(Window);
		}
	}
}

void CEWindowsWindow::Minimize() {
	Assert(Window != 0);

	if (WndStyle.IsMinimizable()) {
		if (IsValid()) {
			ShowWindow(Window, SW_MINIMIZE);
		}
	}
}

void CEWindowsWindow::Maximize() {
	if (WndStyle.IsMaximizable()) {
		if (IsValid()) {
			ShowWindow(Window, SW_MAXIMIZE);
		}
	}
}

void CEWindowsWindow::Restore() {
	Assert(Window != 0);

	if (IsValid()) {
		bool result = ::IsIconic(Window);
		if (result) {
			::ShowWindow(Window, SW_RESTORE);
		}
	}
}

void CEWindowsWindow::ToggleFullscreen() {
	Assert(Window != 0);

	if (IsValid()) {
		if (!IsFullscreen) {
			IsFullscreen = true;

			::GetWindowPlacement(Window, &StoredPlacement);
			if (Style == 0) {
				Style = ::GetWindowLong(Window, GWL_STYLE);
			}
			if (StyleEx == 0) {
				StyleEx = ::GetWindowLong(Window, GWL_EXSTYLE);
			}

			LONG newStyle = Style;
			newStyle &= ~WS_BORDER;
			newStyle &= ~WS_DLGFRAME;
			newStyle &= ~WS_THICKFRAME;

			LONG newStyleEx = StyleEx;
			newStyleEx &= ~WS_EX_WINDOWEDGE;

			SetWindowLong(Window, GWL_STYLE, newStyle | WS_POPUP);
			SetWindowLong(Window, GWL_EXSTYLE, newStyleEx | WS_EX_TOPMOST);
			ShowWindow(Window, SW_SHOWMAXIMIZED);
		}
		else {
			IsFullscreen = false;

			SetWindowLong(Window, GWL_STYLE, Style);
			SetWindowLong(Window, GWL_EXSTYLE, StyleEx);
			ShowWindow(Window, SW_SHOWNORMAL);
			SetWindowPlacement(Window, &StoredPlacement);
		}
	}
}

bool CEWindowsWindow::IsValid() const {
	return IsWindow(Window) == TRUE;
}

bool CEWindowsWindow::IsActiveWindow() const {
	HWND hActive = GetForegroundWindow();
	return (hActive == Window);
}

void CEWindowsWindow::SetTitle(const std::string& Title) {
	Assert(Window != 0);

	if (WndStyle.IsTitled()) {
		if (IsValid()) {
			SetWindowTextA(Window, Title.c_str());
		}
	}
}

void CEWindowsWindow::GetTitle(std::string& OutTitle) {
	if (IsValid()) {
		int32 Size = GetWindowTextLengthA(Window);
		OutTitle.resize(Size);

		GetWindowTextA(Window, OutTitle.data(), Size);
	}
}

void CEWindowsWindow::SetWindowShape(const CEWindowShape& Shape, bool Move) {
	Assert(Window != 0);

	if (IsValid()) {
		UINT Flags = SWP_NOZORDER | SWP_NOACTIVATE;
		if (!Move) {
			Flags |= SWP_NOMOVE;
		}

		SetWindowPos(Window, NULL, Shape.Position.x, Shape.Position.y, Shape.Width, Shape.Height, Flags);
	}
}

void CEWindowsWindow::GetWindowShape(CEWindowShape& OutWindowShape) const {
	Assert(Window != 0);

	if (IsValid()) {
		int32 x = 0;
		int32 y = 0;
		uint32 Width = 0;
		uint32 Height = 0;

		RECT Rect = {};
		if (GetWindowRect(Window, &Rect) != 0) {
			x = static_cast<int32>(Rect.left);
			y = static_cast<int32>(Rect.top);
		}

		if (GetClientRect(Window, &Rect) != 0) {
			Width = static_cast<uint32>(Rect.right - Rect.left);
			Height = static_cast<uint32>(Rect.bottom - Rect.top);
		}

		OutWindowShape = CEWindowShape(Width, Height, x, y);
	}
}

uint32 CEWindowsWindow::GetWidth() const {
	if (IsValid()) {
		RECT Rect = {};
		if (GetClientRect(Window, &Rect) != 0) {
			const uint32 Width = static_cast<uint32>(Rect.right - Rect.left);
			return Width;
		}
	}

	return 0;
}

uint32 CEWindowsWindow::GetHeight() const {
	if (IsValid()) {
		RECT Rect = {};
		if (GetClientRect(Window, &Rect) != 0) {
			const uint32 Height = static_cast<uint32>(Rect.bottom - Rect.top);
			return Height;
		}
	}

	return 0;
}
