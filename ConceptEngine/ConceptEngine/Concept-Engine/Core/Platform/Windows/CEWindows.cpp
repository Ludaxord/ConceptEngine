#include "CEWindows.h"

#include "../../../Utilities/CEUtilities.h"

using namespace ConceptEngine::Core::Platform::Windows;
using namespace ConceptEngine::Core::Platform::Generic::Cursor;
using namespace ConceptEngine::Core::Platform::Generic::Window;
using namespace ConceptEngine::Core::Platform::Generic::Input;

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
	Window = new Window::CEWindowsWindow();
	if (!Window->Create(
			std::string(Title.begin(), Title.end()),
			CEWindowSize::GetWidth(),
			CEWindowSize::GetHeight(),
			Window::DefaultStyle)
	) {
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

bool CEWindows::CreateSystemConsole() {
	Console = new Window::CEWindowsConsole();
	return true;
}

bool CEWindows::CreateCursors() {
	if (!((CECursor::Arrow = Cursor::CEWindowsCursor::Create(IDC_ARROW)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::TextInput = Cursor::CEWindowsCursor::Create(IDC_IBEAM)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeAll = Cursor::CEWindowsCursor::Create(IDC_SIZEALL)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeEW = Cursor::CEWindowsCursor::Create(IDC_SIZEWE)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNS = Cursor::CEWindowsCursor::Create(IDC_SIZENS)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNESW = Cursor::CEWindowsCursor::Create(IDC_SIZENESW)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::ResizeNWSE = Cursor::CEWindowsCursor::Create(IDC_SIZENWSE)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::Hand = Cursor::CEWindowsCursor::Create(IDC_HAND)))) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!((CECursor::NotAllowed = Cursor::CEWindowsCursor::Create(IDC_NO)))) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEWindows::Update() {
	switch (GetEngineBoot()) {
	case EngineBoot::Editor:
		break;
	case EngineBoot::Game:
	case EngineBoot::GameDebug:
		UpdateDefaultGame();
		break;
	default:
		break;
	}
}

void CEWindows::PreInit(HINSTANCE hInstance,
                        LPSTR lpCmdLine,
                        HWND hWnd,
                        int nCmdShow,
                        int width,
                        int height,
                        bool showConsole) {
	Instance = hInstance;
	CmdLine = lpCmdLine;
	HWnd = hWnd;
	CmdShow = nCmdShow;
	Window::CEWindowsWindowSize::Create(width, height);
	Application::CECore::ShowConsole = showConsole;
}

bool CEWindows::CreateInputManager() {
	InputManager = new Input::CEWindowsInputManager();
	return true;
}

bool CEWindows::Release() {
	return true;
}

void CEWindows::SetCapture(CEWindow* window) {
	if (window) {
		Common::CERef<Window::CEWindowsWindow> wWindow = Common::MakeSharedRef<Window::CEWindowsWindow>(window);
		HWND hCapture = wWindow->GetHandle();
		if (wWindow->IsValid()) {
			::SetCapture(hCapture);
		}
	}
	else {
		ReleaseCapture();
	}
}

void CEWindows::SetActiveWindow(CEWindow* window) {
	Common::CERef<Window::CEWindowsWindow> wWindow = Common::MakeSharedRef<Window::CEWindowsWindow>(window);
	HWND hActiveWindow = wWindow->GetHandle();
	if (wWindow->IsValid()) {
		::SetActiveWindow(hActiveWindow);
	}
}

CEWindow* CEWindows::GetCapture() {
	HWND hCapture = ::GetCapture();
	return Window::CEWindowsWindowHandle(hCapture).GetWindow();
}

CEWindow* CEWindows::GetActiveWindow() {
	HWND hActiveWindow = GetForegroundWindow();
	return Window::CEWindowsWindowHandle(hActiveWindow).GetWindow();
}

void CEWindows::SetCursor(CECursor* cursor) {
	if (cursor) {
		// Common::CERef<Cursor::CEWindowsCursor> wCursor = Common::MakeSharedRef<Cursor::CEWindowsCursor>(cursor);
		Cursor = cursor;
		HCURSOR cursorHandle = reinterpret_cast<Cursor::CEWindowsCursor*>(cursor)->GetHandle();
		::SetCursor(cursorHandle);
	}
	else {
		::SetCursor(NULL);
	}
}

CECursor* CEWindows::GetCursor() {
	HCURSOR cursor = ::GetCursor();
	if (Cursor) {
		Cursor::CEWindowsCursor* wCursor = static_cast<Cursor::CEWindowsCursor*>(Cursor);
		if (wCursor->GetHandle() == cursor) {
			Cursor->AddRef();
			return Cursor;
		}
	}

	return nullptr;
}

void CEWindows::SetCursorPosition(CEWindow* relativeWindow, int32 x, int32 y) {
	if (relativeWindow) {
		Common::CERef<Window::CEWindowsWindow> wWindow = Common::MakeSharedRef<Window::CEWindowsWindow>(relativeWindow);
		HWND hRelative = wWindow->GetHandle();
		POINT cursorPos = {x, y};
		if (ClientToScreen(hRelative, &cursorPos)) {
			::SetCursorPos(cursorPos.x, cursorPos.y);
		}
	}
}

void CEWindows::GetCursorPosition(CEWindow* relativeWindow, int32& x, int32& y) {
	POINT cursorPos = {};
	if (!::GetCursorPos(&cursorPos)) {
		return;
	}

	Common::CERef<Window::CEWindowsWindow> wRelative = Common::MakeSharedRef<Window::CEWindowsWindow>(relativeWindow);
	if (wRelative) {
		HWND relative = wRelative->GetHandle();
		if (ScreenToClient(relative, &cursorPos)) {
			x = cursorPos.x;
			y = cursorPos.y;;
		}
	}
}

void CEWindows::UpdateDefaultGame() {
	UpdatePeekMessage();
	UpdateStoredMessage();
}

void CEWindows::UpdatePeekMessage() {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT) {
			StoreMessage(message.hwnd, message.message, message.wParam, message.lParam);
		}
	}
}

void CEWindows::UpdateStoredMessage() {
	for (const Generic::Events::CEEvent& messageEvent : Messages) {
		auto tempEvent = reinterpret_cast<const Events::CEWindowsEvent&>(messageEvent);
		HandleStoredMessage(tempEvent.Window, tempEvent.Message, tempEvent.wParam, tempEvent.lParam);
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

//TODO: Create a link between type of call of engine (Game, GameDebug, Editor) to map controls from user preference (to make actual game)
//NOTE: Use only for Game and Game Debug
void CEWindows::HandleStoredMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	constexpr uint16 SCAN_CODE_MASK = 0x01ff;
	constexpr uint32 KEY_REPEAT_MASK = 0x40000000;
	constexpr uint16 BACK_BUTTON_MASK = 0x0001;

	Common::CERef<Window::CEWindowsWindow> messageWindow = Window::CEWindowsWindowHandle(window).GetWindow();
	switch (message) {
	case WM_CLOSE: {
		break;
	}

	case WM_SETFOCUS:
	case WM_KILLFOCUS: {
		break;
	}

	case WM_MOUSELEAVE: {
		break;
	}

	case WM_SIZE: {
		break;
	}

	case WM_MOVE: {
		break;
	}

	case WM_SYSKEYUP:
	case WM_KEYUP: {
		break;
	}

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN: {
		break;
	}

	case WM_SYSCHAR:
	case WM_CHAR: {
		break;
	}

	case WM_MOUSEMOVE: {
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
		break;
	}
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP: {
		break;
	}

	case WM_MOUSEWHEEL: {
		break;
	}

	case WM_QUIT: {
		break;
	}

	default: {
		break;
	}

	}
}

void CEWindows::StoreMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	Messages.EmplaceBack(Events::CEWindowsEvent(window, message, wParam, lParam));
}

CEModifierKeyState CEWindows::GetModifierKeyState() {
	uint32 modifierMask = 0;
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		modifierMask |= ModifierFlag_Ctrl;
	}
	if (GetKeyState(VK_MENU) & 0x8000) {
		modifierMask |= ModifierFlag_Alt;
	}
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		modifierMask |= ModifierFlag_Shift;
	}
	if (GetKeyState(VK_CAPITAL) & 1) {
		modifierMask |= ModifierFlag_CapsLock;
	}
	if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) {
		modifierMask |= ModifierFlag_Super;
	}
	if (GetKeyState(VK_NUMLOCK) & 1) {
		modifierMask |= ModifierFlag_NumLock;
	}

	return CEModifierKeyState(modifierMask);
}
