#include "CEWindows.h"

#include "Window/CEWindowsConsole.h"
#include "Window/CEWindowsWindow.h"
#include "Input/CEWindowsInputManager.h"
#include "../../Application/CECore.h"
#include "Cursor/CEWindowsCursor.h"

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
	return true;
}

bool CEWindows::CreateSystemConsole() {
	Console = new Window::CEWindowsConsole();
	return true;
}

void CEWindows::CreateCursors() {
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
