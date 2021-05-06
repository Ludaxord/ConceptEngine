#include "CEWindows.h"

#include "Window/CEWindowsConsole.h"
#include "Window/CEWindowsWindow.h"
#include "Input/CEWindowsInputManager.h"

using namespace ConceptEngine::Core::Platform::Windows;

CEWindows::CEWindows(): CEPlatform() {
}

CEWindows::~CEWindows() {
}

void CEWindows::Create() {
}

void CEWindows::CreateSystemWindow() {
	Window = new Window::CEWindowsWindow();
}

void CEWindows::CreateSystemConsole() {
	Console = new Window::CEWindowsConsole();
}

void CEWindows::CreateCursors() {
}

void CEWindows::Update() {
}

bool CEWindows::CreateInputManager() {
	InputManager = new Input::CEWindowsInputManager();
	return true;
}
