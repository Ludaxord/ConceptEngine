#include "CEEngineController.h"

#include "../CEPlatform.h"

#include "../Debug/CEConsoleCommand.h"
#include "../Debug/CETypedConsole.h"

#include "../../../Platform/CEPlatformActions.h"

#include "../../../Application/CECore.h"

using namespace ConceptEngine::Core::Platform::Generic::Callbacks;
using namespace ConceptEngine::Core::Generic::Platform;

//TODO: Move to container
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleCommand ToggleFullscreen;
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleCommand CommExit;

CEEngineController EngineController;

bool CEEngineController::Create() {
	if (Core::Generic::Platform::CEPlatform::GetWindow()) {
		Core::Generic::Platform::CEPlatform::GetWindow()->Show(false);
		ToggleFullscreen.OnExecute.AddObject(Core::Generic::Platform::CEPlatform::GetWindow(),
		                                     &CEWindow::ToggleFullscreen);
		INIT_CONSOLE_COMMAND("CE.ToggleFullscreen", &ToggleFullscreen);
	}
	else {
		CEPlatformActions::MessageBox("Error", "Failed to Initialize Concept Engine");
		return false;
	}

	CommExit.OnExecute.AddObject(this, &CEEngineController::Exit);
	INIT_CONSOLE_COMMAND("CE.Exit", &CommExit);

	return true;
}

bool CEEngineController::Release() {
	return true;
}

void CEEngineController::Exit() {
}

void CEEngineController::OnKeyReleased(CEKey keyCode, const CEModifierKeyState& modifierKeyState) {
	Common::CEKeyReleasedEvent Event(keyCode, modifierKeyState);
	OnKeyReleasedEvent.Broadcast(Event);
}

void CEEngineController::OnKeyPressed(CEKey keyCode, bool isRepeat, const CEModifierKeyState& modifierKeyState) {
	Common::CEKeyPressedEvent Event(keyCode, isRepeat, modifierKeyState);
	OnKeyPressedEvent.Broadcast(Event);
}

void CEEngineController::OnKeyTyped(uint32 character) {
	Common::CEKeyTypedEvent Event(character);
	OnKeyTypedEvent.Broadcast(Event);
}

void CEEngineController::OnMouseMove(int32 x, int32 y) {
	Common::CEMouseMovedEvent Event(x, y);
	OnMouseMoveEvent.Broadcast(Event);
}

void CEEngineController::OnMouseReleased(CEMouseButton button, const CEModifierKeyState& modifierKeyState) {
	if (Application::CECore::GetPlatform()) {
		CEWindow* captureWindow = Application::CECore::GetPlatform()->GetCapture();
		if (captureWindow) {
			Application::CECore::GetPlatform()->SetCapture(nullptr);
		}
	}
}

void CEEngineController::OnMousePressed(CEMouseButton button, const CEModifierKeyState& modifierKeyState) {
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
