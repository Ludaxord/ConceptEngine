#pragma once
#include "../../Core/Application/Events.h"
#include "../../Managers/CEManager.h"

class CEInputManager : public CEManager {
public:
	CEInputManager();
	~CEInputManager() override;
	
	bool Create() override;

	bool IsKeyUp(EKey KeyCode);
	bool IsKeyDown(EKey KeyCode);

	EKey ConvertFromScanCode(uint32 ScanCode);
	uint32 ConvertToScanCode(EKey Key);

	static CEInputManager& Get();

private:
	void InitKeyTable();

	void OnKeyPressed(const KeyPressedEvent& Event);
	void OnKeyReleased(const KeyReleasedEvent& Event);

	void OnWindowFocusChanged(const WindowFocusChangedEvent& Event);

	CEStaticArray<bool, EKey::Key_Count> KeyStates;
	CEStaticArray<EKey, 512> ScanCodeTable;
	CEStaticArray<uint16, 512> KeyTable;
};
