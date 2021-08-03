#include "CEWindows.h"

#include "Core/Application/Windows/CEWindowsConsole.h"
#include "Core/Application/Windows/CEWindowsCursor.h"
#include "Core/Application/Windows/CEWindowsInputManager.h"
#include "Core/Application/Windows/CEWindowsWindow.h"
#include "Core/Application/Windows/CEWindowsPlatform.h"

// CEArray<CEWindowsEvent> CEWindows::Messages;

// CERef<CEWindowsCursor> CEWindows::Cursor;

CEWindows::CEWindows(): CEPlatform() {
}

CEWindows::~CEWindows() {
}

bool CEWindows::Create() {
	if (!CEPlatform::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEWindows::CreateSystemWindow() {
	Window = new CEWindowsWindow();
	if (!Window->Create(ProjectConfig.PTitle, CEWindowSize::GetWidth(), CEWindowSize::GetHeight(),
	                    DefaultStyle)) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool CEWindows::CreateSystemConsole() {
	Console = new CEWindowsConsole();
	return true;
}

bool CEWindows::CreateCursor() {
	if (!((CECursor::Arrow = CEWindowsCursor::Create(IDC_ARROW)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::TextInput = CEWindowsCursor::Create(IDC_IBEAM)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeAll = CEWindowsCursor::Create(IDC_SIZEALL)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeEW = CEWindowsCursor::Create(IDC_SIZEWE)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNS = CEWindowsCursor::Create(IDC_SIZENS)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNESW = CEWindowsCursor::Create(IDC_SIZENESW)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNWSE = CEWindowsCursor::Create(IDC_SIZENWSE)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::Hand = CEWindowsCursor::Create(IDC_HAND)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::NotAllowed = CEWindowsCursor::Create(IDC_NO)))) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEWindows::Update() {
	switch (GEngineConfig.EngineBoot) {

	case EngineBoot::Runtime:
	case EngineBoot::DebugRuntime:
		UpdateRuntime();
		break;
	case EngineBoot::Editor: break;
	case EngineBoot::None: break;
	default: ;
	}
}

bool CEWindows::Release() {

	return true;
}

void CEWindows::SetCapture(CEWindow* ActiveWindow) {
	if (Window) {
		CERef<CEWindowsWindow> WinWindow = MakeSharedRef<CEWindowsWindow>(ActiveWindow);
		HWND hCapture = WinWindow->GetHandle();
		if (WinWindow->IsValid()) {
			::SetCapture(hCapture);
		}
	}
	else {
		ReleaseCapture();
	}
}

void CEWindows::SetActiveWindow(CEWindow* ActiveWindow) {
	CERef<CEWindowsWindow> WinWindow = MakeSharedRef<CEWindowsWindow>(ActiveWindow);
	HWND hActiveWindow = WinWindow->GetHandle();
	if (WinWindow->IsValid()) {
		::SetActiveWindow(hActiveWindow);
	}
}

void CEWindows::SetCursor(CECursor* InCursor) {
	if (Cursor) {
		CERef<CEWindowsCursor> WinCursor = MakeSharedRef<CEWindowsCursor>(InCursor);
		Cursor = WinCursor;
		HCURSOR Cursorhandle = WinCursor->GetHandle();
		::SetCursor(Cursorhandle);
	}
	else {
		::SetCursor(NULL);
	}
}

void CEWindows::SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) {
	if (RelativeWindow) {
		CERef<CEWindowsWindow> WinWindow = MakeSharedRef<CEWindowsWindow>(RelativeWindow);
		HWND hRelative = WinWindow->GetHandle();
		POINT CursorPos = {X, Y};
		if (ClientToScreen(hRelative, &CursorPos)) {
			::SetCursorPos(CursorPos.x, CursorPos.y);
		}
	}
}

CEWindow* CEWindows::GetCapture() {
	HWND hCapture = ::GetCapture();
	return CEWindowsWindowHandle(hCapture).GetWindow();
}

CEWindow* CEWindows::GetActiveWindow() {
	HWND hActiveWindow = GetForegroundWindow();
	return CEWindowsWindowHandle(hActiveWindow).GetWindow();
}

CECursor* CEWindows::GetCursor() {
	HCURSOR cursor = ::GetCursor();
	if (Cursor) {
		CEWindowsCursor* WinCursor = static_cast<CEWindowsCursor*>(Cursor.Get());
		if (WinCursor->GetHandle() == cursor) {
			Cursor->AddRef();
			return Cursor.Get();
		}
		else {
			Cursor.Reset();
		}
	}

	return nullptr;
}

void CEWindows::GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) {
	POINT CursorPos = {};
	if (!::GetCursorPos(&CursorPos)) {
		return;
	}

	CERef<CEWindowsWindow> WinRelative = MakeSharedRef<CEWindowsWindow>(RelativeWindow);
	if (WinRelative) {
		HWND Relative = WinRelative->GetHandle();
		if (::ScreenToClient(Relative, &CursorPos)) {
			X = CursorPos.x;
			Y = CursorPos.y;
		}
	}
}

CEModifierKeyState CEWindows::GetModifierKeyState() {
	uint32 ModifierMask = 0;
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		ModifierMask |= ModifierFlag_Ctrl;
	}
	if (GetKeyState(VK_MENU) & 0x8000) {
		ModifierMask |= ModifierFlag_Alt;
	}
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		ModifierMask |= ModifierFlag_Shift;
	}
	if (GetKeyState(VK_CAPITAL) & 1) {
		ModifierMask |= ModifierFlag_CapsLock;
	}
	if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) {
		ModifierMask |= ModifierFlag_Super;
	}
	if (GetKeyState(VK_NUMLOCK) & 1) {
		ModifierMask |= ModifierFlag_NumLock;
	}

	return CEModifierKeyState(ModifierMask);
}

void CEWindows::PreInit(HINSTANCE hInstance, LPSTR lpCmdLine, HWND hWnd, int nCmdShow, int width, int height) {
	ProjectConfig.PInstance = hInstance;
	ProjectConfig.PCmdLine = lpCmdLine;
	ProjectConfig.PHWnd = hWnd;
	ProjectConfig.PCmdShow = nCmdShow;
	CEWindowsWindowSize::Create(width, height);
}

bool CEWindows::CreateInputManager() {
	InputManager = new CEWindowsInputManager();
	return true;
}

void CEWindows::UpdateRuntime() {
	UpdatePeekMessage();
	UpdateStoredMessage();
}

void CEWindows::UpdatePeekMessage() {
	MSG Message;
	while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&Message);
		DispatchMessage(&Message);

		if (Message.message == WM_QUIT) {
			StoreMessage(Message.hwnd, Message.message, Message.wParam, Message.lParam);
		}
	}
}

void CEWindows::UpdateStoredMessage() {
	for (const CEWindowsEvent& MessageEvent : Messages) {
		auto TempEvent = reinterpret_cast<const CEWindowsEvent&>(MessageEvent);
		HandleStoredMessage(TempEvent.Window, TempEvent.Message, TempEvent.wParam, TempEvent.lParam);
	}

	Messages.Clear();
}

LRESULT CEWindows::MessageProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CLOSE:
	case WM_MOVE:
	case WM_MOUSELEAVE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
	case WM_SIZE:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_SYSCHAR:
	case WM_CHAR:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL: {
		StoreMessage(window, message, wParam, lParam);
	}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

void CEWindows::HandleStoredMessage(HWND ActiveWindow, UINT Message, WPARAM wParam, LPARAM lParam) {
	constexpr uint16 SCAN_CODE_MASK = 0x01ff;
	constexpr uint32 KEY_REPEAT_MASK = 0x40000000;
	constexpr uint16 BACK_BUTTON_MASK = 0x0001;

	CERef<CEWindowsWindow> MessageWindow = CEWindowsWindowHandle(ActiveWindow).GetWindow();
	switch (Message) {

	case WM_CLOSE: {
		if (MessageWindow) {
			Callbacks->OnWindowClosed(MessageWindow);
		}
		break;
	}

	case WM_SETFOCUS:
	case WM_KILLFOCUS: {
		if (MessageWindow) {
			const bool HasFocus = (Message == WM_SETFOCUS);
			Callbacks->OnWindowFocusChanged(MessageWindow, HasFocus);
		}
		break;
	}

	case WM_MOUSELEAVE: {
		if (MessageWindow) {
			Callbacks->OnWindowMouseLeft(MessageWindow);
		}

		bIsMouseTracking = false;
		break;
	}

	case WM_SIZE: {
		if (MessageWindow) {
			const uint16 Width = LOWORD(lParam);
			const uint16 Height = HIWORD(lParam);
			Callbacks->OnWindowResized(MessageWindow, Width, Height);
		}
		break;
	}

	case WM_MOVE: {
		if (MessageWindow) {
			const int16 X = (int16)LOWORD(lParam);
			const int16 Y = (int16)HIWORD(lParam);
			Callbacks->OnWindowMoved(MessageWindow, X, Y);
		}
		break;
	}

	case WM_SYSKEYUP:
	case WM_KEYUP: {
		const uint32 ScanCode = static_cast<uint32>(HIWORD(lParam) & SCAN_CODE_MASK);
		const CEKey Key = InputManager->ConvertFromScanCode(ScanCode);
		Callbacks->OnKeyReleased(Key, GetModifierKeyState());
		break;
	}

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN: {
		const bool IsRepeat = !!(lParam & KEY_REPEAT_MASK);
		const uint32 ScanCode = static_cast<uint32>(HIWORD(lParam) & SCAN_CODE_MASK);
		const CEKey Key = InputManager->ConvertFromScanCode(ScanCode);
		Callbacks->OnKeyPressed(Key, IsRepeat, GetModifierKeyState());
		break;
	}

	case WM_SYSCHAR:
	case WM_CHAR: {
		const uint32 Character = static_cast<uint32>(wParam);
		Callbacks->OnKeyTyped(Character);
		break;
	}

	case WM_MOUSEMOVE: {
		const int32 X = GET_X_LPARAM(lParam);
		const int32 Y = GET_Y_LPARAM(lParam);
		if (!bIsMouseTracking) {
			TRACKMOUSEEVENT TrackEvent;
			CEMemory::Memzero(&TrackEvent);

			TrackEvent.cbSize = sizeof(TRACKMOUSEEVENT);
			TrackEvent.dwFlags = TME_LEAVE;
			TrackEvent.hwndTrack = ActiveWindow;
			::TrackMouseEvent(&TrackEvent);

			Callbacks->OnWindowMouseEntered(MessageWindow);

			bIsMouseTracking = true;
		}

		Callbacks->OnMouseMove(X, Y);
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK: {
		CEMouseButton Button = CEMouseButton::MouseButton_Unknown;
		if (Message == WM_LBUTTONDOWN || Message == WM_LBUTTONDBLCLK) {
			Button = CEMouseButton::MouseButton_Left;
		}
		else if (Message == WM_MBUTTONDOWN || Message == WM_MBUTTONDBLCLK) {
			Button = CEMouseButton::MouseButton_Middle;
		}
		else if (Message == WM_RBUTTONDOWN || Message == WM_RBUTTONDBLCLK) {
			Button = CEMouseButton::MouseButton_Right;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == BACK_BUTTON_MASK) {
			Button = CEMouseButton::MouseButton_Back;
		}
		else {
			Button = CEMouseButton::MouseButton_Forward;
		}

		Callbacks->OnMousePressed(Button, GetModifierKeyState());

		break;
	}
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP: {
		CEMouseButton Button = CEMouseButton::MouseButton_Unknown;
		if (Message == WM_LBUTTONUP) {
			Button = CEMouseButton::MouseButton_Left;
		}
		else if (Message == WM_MBUTTONUP) {
			Button = CEMouseButton::MouseButton_Middle;
		}
		else if (Message == WM_RBUTTONUP) {
			Button = CEMouseButton::MouseButton_Right;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == BACK_BUTTON_MASK) {
			Button = CEMouseButton::MouseButton_Back;
		}
		else {
			Button = CEMouseButton::MouseButton_Forward;
		}

		Callbacks->OnMouseReleased(Button, GetModifierKeyState());
		break;
	}

	case WM_MOUSEWHEEL: {
		const float WheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam) / static_cast<float>(WHEEL_DELTA));
		Callbacks->OnMouseScrolled(0.0f, WheelDelta);
		break;
	}

	case WM_MOUSEHWHEEL: {
		const float WheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);
		Callbacks->OnMouseScrolled(WheelDelta, 0.0f);
		break;
	}

	case WM_QUIT: {
		int32 ExitCode = (int32)wParam;
		Callbacks->OnApplicationExit(ExitCode);
		break;
	}

	default: {
		break;
	}

	}
}


void CEWindows::StoreMessage(HWND ActiveWindow, UINT Message, WPARAM wParam, LPARAM lParam) {
	Messages.EmplaceBack(CEWindowsEvent(ActiveWindow, Message, wParam, lParam));
}


CEWindow* CEWindows::GetWindow() {
	return Window.Get();
}
