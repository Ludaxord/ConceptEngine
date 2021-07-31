#pragma once
#include "../../Core/Application/Events.h"
#include "../../Managers/CEManager.h"

class CEInputManager : public CEManager {
public:
	CEInputManager();
	~CEInputManager() override;
	
	bool Create() override;

	bool IsKeyUp(CEKey KeyCode);
	bool IsKeyDown(CEKey KeyCode);

	CEKey ConvertFromScanCode(uint32 ScanCode);
	uint32 ConvertToScanCode(CEKey Key);

	static CEInputManager& Get();

private:
	void InitKeyTable();

	void OnKeyPressed(const KeyPressedEvent& Event);
	void OnKeyReleased(const KeyReleasedEvent& Event);

	void OnWindowFocusChanged(const WindowFocusChangedEvent& Event);

	CEStaticArray<bool, CEKey::Key_Count> KeyStates;
	CEStaticArray<CEKey, 512> ScanCodeTable;
	CEStaticArray<uint16, 512> KeyTable;
};
