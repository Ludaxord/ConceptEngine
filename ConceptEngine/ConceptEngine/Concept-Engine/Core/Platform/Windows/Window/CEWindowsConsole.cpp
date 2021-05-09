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
	if (ConsoleHandle) {
		Core::Threading::CEScopedLock<CEMutex> Lock(ConsoleMutex);
		WriteConsoleA(ConsoleHandle, message.c_str(), static_cast<DWORD>(message.size()), 0, NULL);
	}
}

void CEWindowsConsole::Clear() {
	if (ConsoleHandle) {
		Core::Threading::CEScopedLock<CEMutex> Lock(ConsoleMutex);

		CONSOLE_SCREEN_BUFFER_INFO CSBI;
		Memory::CEMemory::Memzero(&CSBI);
		if (GetConsoleScreenBufferInfo(ConsoleHandle, &CSBI)) {
			COORD Dest = {0, -CSBI.dwSize.Y};
			CHAR_INFO FillInfo = {'\0', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};
			ScrollConsoleScreenBufferA(ConsoleHandle, &CSBI.srWindow, nullptr, Dest, &FillInfo);

			COORD CursorPos = {0, 0};
			SetConsoleCursorPosition(ConsoleHandle, CursorPos);
		}
	}
}

void CEWindowsConsole::SetTitle(const std::string& title) {
	if (ConsoleHandle) {
		Core::Threading::CEScopedLock<CEMutex> Lock(ConsoleMutex);
		SetConsoleTitleA(title.c_str());
	}
}

void CEWindowsConsole::SetColor(Generic::Window::CEConsoleColor color) {
	if (ConsoleHandle) {
		Core::Threading::CEScopedLock<CEMutex> Lock(ConsoleMutex);
		WORD wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		switch (color) {
		case Generic::Window::CEConsoleColor::Red:
			wColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
		case Generic::Window::CEConsoleColor::Green:
			wColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case Generic::Window::CEConsoleColor::Yellow:
			wColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case Generic::Window::CEConsoleColor::White:
			break;
		}

		SetConsoleTextAttribute(ConsoleHandle, wColor);
	}
}
