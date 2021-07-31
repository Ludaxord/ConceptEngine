#include "CEInputManager.h"

#include "../../Core/Engine/Engine.h"

CEInputManager::CEInputManager(): CEManager() {
}

CEInputManager::~CEInputManager() {
}

bool CEInputManager::Create() {
	InitKeyTable();

	GEngine.OnKeyPressedEvent.AddObject(this, &CEInputManager::OnKeyPressed);
	GEngine.OnKeyReleasedEvent.AddObject(this, &CEInputManager::OnKeyReleased);
	GEngine.OnWindowFocusChangedEvent.AddObject(this, &CEInputManager::OnWindowFocusChanged);

	return true;
}

bool CEInputManager::IsKeyUp(CEKey KeyCode) {
	return !KeyStates[KeyCode];
}

bool CEInputManager::IsKeyDown(CEKey KeyCode) {
	return KeyStates[KeyCode];
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

void CEInputManager::OnWindowFocusChanged(const WindowFocusChangedEvent& Event) {
	if (!Event.HasFocus) {
		KeyStates.Fill(false);
	}
}
