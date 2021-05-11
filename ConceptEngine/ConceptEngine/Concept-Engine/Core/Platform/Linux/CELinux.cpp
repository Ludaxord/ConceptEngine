#include "CELinux.h"

using namespace ConceptEngine::Core::Platform::Linux;

CELinux::CELinux() : CEPlatform() {
}

CELinux::~CELinux() {
}

bool CELinux::Create() {
	return true;
}

bool CELinux::CreateSystemWindow() {
	return true;
}

bool CELinux::CreateSystemConsole() {
	return true;
}

bool CELinux::CreateCursors() {
	return true;
}

void CELinux::Update() {
}

void CELinux::PreInit() {
}

bool CELinux::Release() {
	return true;
}

ConceptEngine::Core::Platform::Generic::Input::CEModifierKeyState CELinux::GetModifierKeyState() {
	return Generic::Input::CEModifierKeyState();
}

void CELinux::SetCapture(Generic::Window::CEWindow* window) {
}

void CELinux::SetActiveWindow(Generic::Window::CEWindow* window) {
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CELinux::GetCapture() {
	return nullptr;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CELinux::GetActiveWindow() {
	return nullptr;
}

void CELinux::SetCursor(Generic::Cursor::CECursor* cursor) {
}

ConceptEngine::Core::Platform::Generic::Cursor::CECursor* CELinux::GetCursor() {
	return nullptr;
}

void CELinux::SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

void CELinux::GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32& x, int32& y) {
}

bool CELinux::CreateInputManager() {
	return true;
}
