#include "CEAndroid.h"

using namespace ConceptEngine::Core::Platform::Android;

CEAndroid::CEAndroid() : CEPlatform() {
}

CEAndroid::~CEAndroid() {
}

bool CEAndroid::Create() {
	return true;
}

bool CEAndroid::CreateSystemWindow() {
	return true;
}

bool CEAndroid::CreateSystemConsole() {
	return true;
}

bool CEAndroid::CreateCursors() {
	return true;
}

void CEAndroid::Update() {
}

void CEAndroid::PreInit() {
}

bool CEAndroid::Release() {
	return true;
}

ConceptEngine::Core::Platform::Generic::Input::CEModifierKeyState CEAndroid::GetModifierKeyState() {
	return Generic::Input::CEModifierKeyState();
}

void CEAndroid::SetCapture(Generic::Window::CEWindow* window) {
}

void CEAndroid::SetActiveWindow(Generic::Window::CEWindow* window) {
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEAndroid::GetCapture() {
	return nullptr;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEAndroid::GetActiveWindow() {
	return nullptr;
}

void CEAndroid::SetCursor(Generic::Cursor::CECursor* cursor) {
}

ConceptEngine::Core::Platform::Generic::Cursor::CECursor* CEAndroid::GetCursor() {
	return nullptr;
}

void CEAndroid::SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

void CEAndroid::GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

bool CEAndroid::CreateInputManager() {
	return true;
}
