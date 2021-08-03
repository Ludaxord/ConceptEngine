#pragma once
#include "../../Core/Application/Events.h"
#include "../../Managers/CEManager.h"

#include "../../Core/Containers/StaticArray.h"

class CEInputManager : public CEManager {
public:
	CEInputManager();
	~CEInputManager() override;

	bool Create() override;

	bool IsKeyUp(CEKey KeyCode);
	bool IsKeyDown(CEKey KeyCode);

	CEKey ConvertFromScanCode(uint32 ScanCode);
	uint32 ConvertToScanCode(CEKey Key);

private:
	virtual void InitKeyTable() = 0;

	void OnKeyPressed(const KeyPressedEvent& Event);
	void OnKeyReleased(const KeyReleasedEvent& Event);

	void OnWindowFocusChanged(const WindowFocusChangedEvent& Event);

public:
	void Release() override;
protected:
	CEStaticArray<bool, CEKey::Key_Count> KeyStates{};
	CEStaticArray<CEKey, 512> ScanCodeTable{};
	CEStaticArray<uint16, 512> KeyTable{};
};
