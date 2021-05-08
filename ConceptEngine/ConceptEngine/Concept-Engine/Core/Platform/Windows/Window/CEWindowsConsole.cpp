#include "CEWindowsConsole.h"

using namespace ConceptEngine::Core::Platform::Windows::Window;

CEWindowsConsole::CEWindowsConsole(): CEConsole(), ConsoleHandle(0) {
	if (AllocConsole()) {
		ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitleA("Windows Console");
	}
}

CEWindowsConsole::~CEWindowsConsole() {
	if (ConsoleHandle) {
		FreeConsole();
		ConsoleHandle = 0;
	}
}

bool CEWindowsConsole::Create() {
	return true;
}

void CEWindowsConsole::Update() {
}

void CEWindowsConsole::Print(const std::string& message) {
}

void CEWindowsConsole::Clear() {
}

void CEWindowsConsole::SetTitle(const std::string& title) {
}

void CEWindowsConsole::SetColor(Generic::Window::CEConsoleColor color) {
}
