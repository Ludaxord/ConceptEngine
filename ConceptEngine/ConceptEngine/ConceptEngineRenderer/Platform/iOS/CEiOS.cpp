#include "CEiOS.h"

CEiOS::CEiOS(): CEPlatform() {
}

CEiOS::~CEiOS() {
}

bool CEiOS::CreateSystemWindow() {
	return false;
}

bool CEiOS::CreateSystemConsole() {
	return false;
}

bool CEiOS::CreateCursor() {
	return false;
}

void CEiOS::Update() {
}

bool CEiOS::Release() {
	return false;
}

void CEiOS::SetCapture(CEWindow* Window) {
}

void CEiOS::SetActiveWindow(CEWindow* Window) {
}

void CEiOS::SetCursor(CECursor* Cursor) {
}

CEWindow* CEiOS::GetCapture() {
	return nullptr;
}

CEWindow* CEiOS::GetActiveWindow() {
	return nullptr;
}

CECursor* CEiOS::GetCursor() {
	return nullptr;
}

bool CEiOS::CreateInputManager() {
	return false;
}

void CEiOS::SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) {
}

void CEiOS::GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) {
}

CEWindow* CEiOS::GetWindow() {
	return nullptr;
}
