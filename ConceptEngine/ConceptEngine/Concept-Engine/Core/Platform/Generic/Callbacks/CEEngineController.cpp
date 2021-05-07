#include "CEEngineController.h"

using namespace ConceptEngine::Core::Platform::Generic::Callbacks;

CEEngineController EngineController;

bool CEEngineController::Create() {
	return true;
}

bool CEEngineController::Release() {
	return true;
}

bool CEEngineController::Exit() {
	return true;
}

void CEEngineController::OnKeyReleased(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) {
}

void CEEngineController::OnKeyPressed(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) {
}

void CEEngineController::OnKeyTyped(uint32 character) {
}

void CEEngineController::OnMouseMove(int32 x, int32 y) {
}

void CEEngineController::
OnMouseReleased(Input::CEMouseButton button, const Input::CEModifierKeyState& modifierKeyState) {
}

void CEEngineController::
OnMousePressed(Input::CEMouseButton button, const Input::CEModifierKeyState& modifierKeyState) {
}

void CEEngineController::OnMouseScrolled(float horizontalDelta, float verticalDelta) {
}

void CEEngineController::OnWindowResized(const Common::CERef<Window::CEWindow>& window, uint16 width, uint16 height) {
}

void CEEngineController::OnWindowMoved(const Common::CERef<Window::CEWindow>& window, uint16 x, uint16 y) {
}

void CEEngineController::OnWindowFocusChanged(const Common::CERef<Window::CEWindow>& window, bool hasFocus) {
}

void CEEngineController::OnWindowMouseLeft(const Common::CERef<Window::CEWindow>& window) {
}

void CEEngineController::OnWindowMouseEntered(const Common::CERef<Window::CEWindow>& window) {
}

void CEEngineController::OnWindowClosed(const Common::CERef<Window::CEWindow>& window) {
}

void CEEngineController::OnApplicationExit(int32 exitCode) {
}
