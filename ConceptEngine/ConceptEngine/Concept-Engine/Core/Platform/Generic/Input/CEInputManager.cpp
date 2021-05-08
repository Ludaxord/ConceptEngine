#include "CEInputManager.h"

#include "../Callbacks/CEEngineController.h"

using namespace ConceptEngine::Core::Platform::Generic::Input;

CEInputManager::CEInputManager() : CEManager() {
}

CEInputManager::~CEInputManager() {
}

bool CEInputManager::Create() {
	InitKeyTable();

	/*
	 * NOTE: EngineController works in similar way as slot, signal in QT Framework
	 */
	Callbacks::EngineController.OnKeyPressedEvent.AddObject(this, &CEInputManager::OnKeyPressed);
	Callbacks::EngineController.OnKeyReleasedEvent.AddObject(this, &CEInputManager::OnKeyReleased);
	Callbacks::EngineController.OnWindowFocusChangedEvent.AddObject(this, &CEInputManager::OnWindowFocusChanged);
	return true;
}

bool CEInputManager::IsKeyUp(CEKey keyCode) {
	return !KeyStates[keyCode];
}

bool CEInputManager::IsKeyDown(CEKey keyCode) {
	return KeyStates[keyCode];
}

CEKey CEInputManager::ConvertFromScanCode(uint32 scanCode) {
	return ScanCodeTable[scanCode];
}

uint32 CEInputManager::ConvertToScanCode(CEKey key) {
	return KeyTable[key];
}

void CEInputManager::OnKeyPressed(const Common::CEKeyPressedEvent& Event) {
	KeyStates[Event.Key] = true;
}

void CEInputManager::OnKeyReleased(const Common::CEKeyReleasedEvent& Event) {
	KeyStates[Event.Key] = false;
}

void CEInputManager::OnWindowFocusChanged(const Common::CEWindowFocusChangedEvent& Event) {
	if (!Event.HasFocus) {
		KeyStates.Fill(false);
	}
}
