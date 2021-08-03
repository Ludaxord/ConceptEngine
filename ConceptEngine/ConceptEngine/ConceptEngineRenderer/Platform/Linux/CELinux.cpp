#include "CELinux.h"

CELinux::CELinux() : CEPlatform() {
}

CELinux::~CELinux() {
}

bool CELinux::CreateSystemWindow() {
	return false;
}

bool CELinux::CreateSystemConsole() {
	return false;
}

bool CELinux::CreateCursor() {
	return false;
}

void CELinux::Update() {
}

bool CELinux::Release() {
	return false;
}

void CELinux::SetCapture(CEWindow* Window) {
}

void CELinux::SetActiveWindow(CEWindow* Window) {
}

void CELinux::SetCursor(CECursor* Cursor) {
}

CEWindow* CELinux::GetCapture() {
	return nullptr;
}

CEWindow* CELinux::GetActiveWindow() {
	return nullptr;
}

CECursor* CELinux::GetCursor() {
	return nullptr;
}

void CELinux::SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) {
}

void CELinux::GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) {
}

bool CELinux::CreateInputManager() {
	return false;
}

CERef<CEWindow> CELinux::GetWindow() {
	return nullptr;
}
