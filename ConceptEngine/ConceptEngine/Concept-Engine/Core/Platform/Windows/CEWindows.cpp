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
	m_window = new Window::CEWindowsWindow();
}

void CEWindows::CreateSystemConsole() {
	m_console = new Window::CEWindowsConsole();
}

void CEWindows::CreateCursors() {
}

bool CEWindows::CreateInputManager() {
	m_inputManager = new Input::CEWindowsInputManager();
	return true;
}
