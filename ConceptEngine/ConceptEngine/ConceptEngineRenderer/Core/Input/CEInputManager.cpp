#include "CEInputManager.h"

#include "../../Core/Engine/Engine.h"
#include "Boot/Callbacks/CEEngineController.h"

CEInputManager::CEInputManager(): CEManager() {
}

CEInputManager::~CEInputManager() {
}

bool CEInputManager::Create() {
	InitKeyTable();

	// GEngine.OnKeyPressedEvent.AddObject(this, &CEInputManager::OnKeyPressed);
	// GEngine.OnKeyReleasedEvent.AddObject(this, &CEInputManager::OnKeyReleased);
	// GEngine.OnWindowFocusChangedEvent.AddObject(this, &CEInputManager::OnWindowFocusChanged);

	GEngineController.OnKeyPressedEvent.AddObject(this, &CEInputManager::OnKeyPressed);
	GEngineController.OnKeyReleasedEvent.AddObject(this, &CEInputManager::OnKeyReleased);
	GEngineController.OnWindowFocusChangedEvent.AddObject(this, &CEInputManager::OnWindowFocusChanged);

	GEngineController.OnMousePressedEvent.AddObject(this, &CEInputManager::OnMousePressed);
	GEngineController.OnMouseReleasedEvent.AddObject(this, &CEInputManager::OnMouseReleased);
	GEngineController.OnMouseMoveEvent.AddObject(this, &CEInputManager::OnMouseMoved);

	return true;
}

bool CEInputManager::IsKeyUp(CEKey KeyCode) {
	return !KeyStates[KeyCode];
}

bool CEInputManager::IsKeyDown(CEKey KeyCode) {
	return KeyStates[KeyCode];
}

bool CEInputManager::IsMouseDown(CEMouseButton MouseButton) {
	return MouseButtonKeyStates[MouseButton];
}

std::pair<int32, int32> CEInputManager::IsMouseMovement() {
	return std::pair(MouseXOffset, MouseYOffset);
}

CEKey CEInputManager::ConvertFromScanCode(uint32 ScanCode) {
	return ScanCodeTable[ScanCode];
}

uint32 CEInputManager::ConvertToScanCode(CEKey KeyCode) {
	return KeyTable[KeyCode];
}

void CEInputManager::OnKeyPressed(const KeyPressedEvent& Event) {
	// TODO: Maybe a better solution that this?
	ImGuiIO& IO = ImGui::GetIO();
	if (!IO.WantCaptureKeyboard) {
		KeyStates[Event.Key] = true;
	}
}

void CEInputManager::OnKeyReleased(const KeyReleasedEvent& Event) {
	KeyStates[Event.Key] = false;
}

void CEInputManager::OnMousePressed(const MousePressedEvent& Event) {
	MouseButtonKeyStates[Event.Button] = true;
}

void CEInputManager::OnMouseReleased(const MouseReleasedEvent& Event) {
	MouseButtonKeyStates[Event.Button] = false;
}

void CEInputManager::OnMouseMoved(const MouseMovedEvent& Event) {
	MouseXOffset = Event.x;
	MouseYOffset = Event.y;
}

void CEInputManager::OnWindowFocusChanged(const WindowFocusChangedEvent& Event) {
	if (!Event.HasFocus) {
		KeyStates.Fill(false);
	}
}

void CEInputManager::Release() {
}
