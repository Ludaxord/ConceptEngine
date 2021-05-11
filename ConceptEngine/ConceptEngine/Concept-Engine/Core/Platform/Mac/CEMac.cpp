#include "CEMac.h"

using namespace ConceptEngine::Core::Platform::Mac;

CEMac::CEMac(): CEPlatform() {
}

CEMac::~CEMac() {
}

bool CEMac::Create() {
	return true;
}

bool CEMac::CreateSystemWindow() {
	return true;
}

bool CEMac::CreateSystemConsole() {
	return true;
}

bool CEMac::CreateCursors() {
	return true;
}

void CEMac::Update() {
}

void CEMac::PreInit() {
}

bool CEMac::Release() {
	return true;
}

ConceptEngine::Core::Platform::Generic::Input::CEModifierKeyState CEMac::GetModifierKeyState() {
	return Generic::Input::CEModifierKeyState();
}

void CEMac::SetCapture(Generic::Window::CEWindow* window) {
}

void CEMac::SetActiveWindow(Generic::Window::CEWindow* window) {
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEMac::GetCapture() {
	return nullptr;
}

ConceptEngine::Core::Platform::Generic::Window::CEWindow* CEMac::GetActiveWindow() {
	return nullptr;
}

void CEMac::SetCursor(Generic::Cursor::CECursor* cursor) {
}

ConceptEngine::Core::Platform::Generic::Cursor::CECursor* CEMac::GetCursor() {
	return nullptr;
}

void CEMac::SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) {
}

void CEMac::GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32& x, int32& y) {
}

bool CEMac::CreateInputManager() {
	return true;
}
