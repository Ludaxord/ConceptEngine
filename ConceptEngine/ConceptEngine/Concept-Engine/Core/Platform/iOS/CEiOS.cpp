#include "CEiOS.h"

#include "../Generic/Input/CEInputManager.h"

using namespace ConceptEngine::Core::Platform::iOS;

CEiOS::CEiOS(): CEPlatform() {
}

CEiOS::~CEiOS() {
}

bool CEiOS::Create() {
	return true;
}

bool CEiOS::CreateSystemWindow() {
	return true;
}

bool CEiOS::CreateSystemConsole() {
	return true;
}

bool CEiOS::CreateCursors() {
	return true;
}

void CEiOS::Update() {
}

void CEiOS::PreInit() {
}

bool CEiOS::Release() {
	return true;
}

ConceptEngine::Core::Platform::Generic::Input::CEModifierKeyState CEiOS::GetModifierKeyState() {
	return Generic::Input::CEModifierKeyState();
}

void CEiOS::SetCapture(Generic::Window::CEWindow* window) {
}

void CEiOS::SetActiveWindow(Generic::Window::CEWindow* window) {
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEiOS::GetCapture() {
	return nullptr;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEiOS::GetActiveWindow() {
	return nullptr;
}

void CEiOS::SetCursor(Generic::Cursor::CECursor* cursor) {
}

ConceptEngine::Core::Platform::Generic::Cursor::CECursor* CEiOS::GetCursor() {
	return nullptr;
}

void CEiOS::SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

void CEiOS::GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

bool CEiOS::CreateInputManager() {
	return true;
}
