#include "CEEngineController.h"

#include "Boot/CECore.h"
#include "Debug/Console/CEConsoleCommand.h"
#include "Debug/Console/Console.h"
#include "Platform/CEPlatform.h"
#include "Core/Types.h"
#include "Core/Delegates/Event.h"
#include "Platform/Generic/Console/CETypedConsole.h"

CEConsoleCommand GToggleFullscreen;
CEConsoleCommand GExit;

CEEngineController GEngineController;

bool CEEngineController::Create() {
	if (CECore::GetPlatform()->GetWindow()->GetNativeHandle()) {
		CECore::GetPlatform()->GetWindow()->Show(false);
		GToggleFullscreen.OnExecute.AddObject(CECore::GetPlatform()->GetWindow().Get(), &CEWindow::ToggleFullscreen);
		INIT_CONSOLE_COMMAND("CE.ToggleFullscreen", &GToggleFullscreen);
	}
	else {
		CEPlatformMisc::MessageBox("ERROR", "Failed to Create Engine Window");
		return false;
	}

	GExit.OnExecute.AddObject(this, &CEEngineController::Exit);
	INIT_CONSOLE_COMMAND("CE.Exit", &GExit);

	bIsRunning = true;
	return true;
}

bool CEEngineController::Release() {
	return false;
}

void CEEngineController::Exit() {
	bIsRunning = false;
}

CEEngineController::~CEEngineController() {
}

void CEEngineController::OnKeyReleased(CEKey KeyCode, const CEModifierKeyState& ModfierKeyState) {
	KeyReleasedEvent Event(KeyCode, ModfierKeyState);
	OnKeyReleasedEvent.Broadcast(Event);
}

void CEEngineController::OnKeyPressed(CEKey KeyCode, bool IsRepeat, const CEModifierKeyState& ModfierKeyState) {
	KeyPressedEvent Event(KeyCode, IsRepeat, ModfierKeyState);
	OnKeyPressedEvent.Broadcast(Event);
}

void CEEngineController::OnKeyTyped(uint32 Character) {
	KeyTypedEvent Event(Character);
	OnKeyTypedEvent.Broadcast(Event);
}

void CEEngineController::OnMouseMove(int32 x, int32 y) {
	MouseMovedEvent Event(x, y);
	OnMouseMoveEvent.Broadcast(Event);
}

void CEEngineController::OnMouseReleased(CEMouseButton Button, const CEModifierKeyState& ModfierKeyState) {
	CEWindow* CaptureWindow = CECore::GetPlatform()->GetCapture();
	if (CaptureWindow) {
		CECore::GetPlatform()->SetCapture(nullptr);
	}

	MouseReleasedEvent Event(Button, ModfierKeyState);
	OnMouseReleasedEvent.Broadcast(Event);
}

void CEEngineController::OnMousePressed(CEMouseButton Button, const CEModifierKeyState& ModfierKeyState) {
	CEWindow* CaptureWindow = CECore::GetPlatform()->GetCapture();
	if (!CaptureWindow) {
		CEWindow* ActiveWindow = CECore::GetPlatform()->GetActiveWindow();
		CECore::GetPlatform()->SetCapture(ActiveWindow);
	}

	MousePressedEvent Event(Button, ModfierKeyState);
	OnMousePressedEvent.Broadcast(Event);
}

void CEEngineController::OnMouseScrolled(float HorizontalDelta, float VerticalDelta) {
	MouseScrolledEvent Event(HorizontalDelta, VerticalDelta);
	OnMouseScrolledEvent.Broadcast(Event);
}

void CEEngineController::OnWindowResized(const CERef<CEWindow>& InWindow, uint16 Width, uint16 Height) {
	WindowResizeEvent Event(InWindow, Width, Height);
	OnWindowResizedEvent.Broadcast(Event);
}

void CEEngineController::OnWindowMoved(const CERef<CEWindow>& InWindow, int16 x, int16 y) {
	WindowMovedEvent Event(InWindow, x, y);
	OnWindowMovedEvent.Broadcast(Event);
}

void CEEngineController::OnWindowFocusChanged(const CERef<CEWindow>& InWindow, bool HasFocus) {
	WindowFocusChangedEvent Event(InWindow, HasFocus);
	OnWindowFocusChangedEvent.Broadcast(Event);
}

void CEEngineController::OnWindowMouseLeft(const CERef<CEWindow>& InWindow) {
	WindowMouseLeftEvent Event(InWindow);
	OnWindowMouseLeftEvent.Broadcast(Event);
}

void CEEngineController::OnWindowMouseEntered(const CERef<CEWindow>& InWindow) {
	WindowMouseEnteredEvent Event(InWindow);
	OnWindowMouseEnteredEvent.Broadcast(Event);
}

void CEEngineController::OnWindowClosed(const CERef<CEWindow>& InWindow) {
	WindowClosedEvent Event(InWindow);
	OnWindowClosedEvent.Broadcast(Event);

	if (InWindow == CECore::GetPlatform()->GetWindow()) {
		CEPlatformMisc::RequestExit(0);
	}
}

void CEEngineController::OnApplicationExit(int32 ExitCode) {
	bIsRunning = false;
	OnApplicationExitEvent.Broadcast(ExitCode);
}
