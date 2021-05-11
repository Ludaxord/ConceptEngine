#include "CEWindows.h"

#include "Window/CEWindowsConsole.h"
#include "Window/CEWindowsWindow.h"
#include "Input/CEWindowsInputManager.h"
#include "../../Application/CECore.h"
#include "Cursor/CEWindowsCursor.h"
#include "Events/CEWindowsEvent.h"

using namespace ConceptEngine::Core::Platform::Windows;

CEWindows::CEWindows(): CEPlatform() {
}

CEWindows::~CEWindows() {
}

bool CEWindows::Create() {
	return true;
}

bool CEWindows::CreateSystemWindow() {
	Window = new Window::CEWindowsWindow();
	if (!Window->Create(
			std::string(Title.begin(), Title.end()),
			Generic::Window::CEWindowSize::GetWidth(),
			Generic::Window::CEWindowSize::GetHeight(),
			Window::DefaultStyle)
	) {
		return false;
	}
	return true;
}

bool CEWindows::CreateSystemConsole() {
	Console = new Window::CEWindowsConsole();
	return true;
}

bool CEWindows::CreateCursors() {
	if (!(Generic::Cursor::CECursor::Arrow = Cursor::CEWindowsCursor::Create(IDC_ARROW))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::TextInput = Cursor::CEWindowsCursor::Create(IDC_IBEAM))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::ResizeAll = Cursor::CEWindowsCursor::Create(IDC_SIZEALL))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::ResizeEW = Cursor::CEWindowsCursor::Create(IDC_SIZEWE))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::ResizeNS = Cursor::CEWindowsCursor::Create(IDC_SIZENS))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::ResizeNESW = Cursor::CEWindowsCursor::Create(IDC_SIZENESW))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::ResizeNWSE = Cursor::CEWindowsCursor::Create(IDC_SIZENWSE))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::Hand = Cursor::CEWindowsCursor::Create(IDC_HAND))) {
		return false;
	}
	if (!(Generic::Cursor::CECursor::NotAllowed = Cursor::CEWindowsCursor::Create(IDC_NO))) {
		return false;
	}

	return true;
}

void CEWindows::Update() {
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

void CEWindows::SetCapture(Generic::Window::CEWindow* window) {
}

void CEWindows::SetActiveWindow(Generic::Window::CEWindow* window) {
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEWindows::GetCapture() {
	return nullptr;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEWindows::GetActiveWindow() {
	return nullptr;
}

void CEWindows::SetCursor(Generic::Cursor::CECursor* cursor) {
}

ConceptEngine::Core::Platform::Generic::Cursor::CECursor* CEWindows::GetCursor() {
	return nullptr;
}

void CEWindows::SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

void CEWindows::GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
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
