#include "CEMac.h"

CEMac::CEMac() : CEPlatform() {
}

CEMac::~CEMac() {
}

bool CEMac::CreateSystemWindow() {
	return false;
}

bool CEMac::CreateSystemConsole() {
	return false;
}

bool CEMac::CreateCursor() {
	return false;
}

void CEMac::Update() {
}

bool CEMac::Release() {
	return false;
}

void CEMac::SetCapture(CEWindow* Window) {
}

void CEMac::SetActiveWindow(CEWindow* Window) {
}

void CEMac::SetCursor(CECursor* Cursor) {
}

CEWindow* CEMac::GetCapture() {
	return nullptr;
}

CEWindow* CEMac::GetActiveWindow() {
	return nullptr;
}

CECursor* CEMac::GetCursor() {
	return nullptr;
}

void CEMac::SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) {
}

void CEMac::GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) {
}

bool CEMac::CreateInputManager() {
	return false;
}

CERef<CEWindow> CEMac::GetWindow() {
	return nullptr;
}
