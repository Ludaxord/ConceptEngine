#include "CEAndroid.h"

CEAndroid::CEAndroid(): CEPlatform() {
}

CEAndroid::~CEAndroid() {
}

bool CEAndroid::CreateSystemWindow() {
	return false;
}

bool CEAndroid::CreateSystemConsole() {
	return false;
}

bool CEAndroid::CreateCursor() {
	return false;
}

void CEAndroid::Update() {
}

bool CEAndroid::Release() {
	return false;
}

void CEAndroid::SetCapture(CEWindow* Window) {
}

void CEAndroid::SetActiveWindow(CEWindow* Window) {
}

void CEAndroid::SetCursor(CECursor* Cursor) {
}

CEWindow* CEAndroid::GetCapture() {
	return nullptr;
}

CEWindow* CEAndroid::GetActiveWindow() {
	return nullptr;
}

CECursor* CEAndroid::GetCursor() {
	return nullptr;
}

bool CEAndroid::CreateInputManager() {
	return false;
}

void CEAndroid::SetCursorPosition(CEWindow* RelativeWindow, int32 X, int32 Y) {
}

void CEAndroid::GetCursorPosition(CEWindow* RelativeWindow, int32& X, int32& Y) {
}

CERef<CEWindow> CEAndroid::GetWindow() {
	return nullptr;
}
