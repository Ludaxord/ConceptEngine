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
	bool IsMouseDown(CEMouseButton MouseButton);
	std::pair<int32, int32> IsMouseMovement();

	CEKey ConvertFromScanCode(uint32 ScanCode);
	uint32 ConvertToScanCode(CEKey Key);

private:
	virtual void InitKeyTable() = 0;

	void OnKeyPressed(const KeyPressedEvent& Event);
	void OnKeyReleased(const KeyReleasedEvent& Event);

	void OnMousePressed(const MousePressedEvent& Event);
	void OnMouseReleased(const MouseReleasedEvent& Event);
	void OnMouseMoved(const MouseMovedEvent& Event);

	void OnWindowFocusChanged(const WindowFocusChangedEvent& Event);

public:
	void Release() override;
protected:
	CEStaticArray<bool, CEKey::Key_Count> KeyStates{};
	CEStaticArray<bool, CEMouseButton::MouseButton_Count> MouseButtonKeyStates{};
	int32 MouseXOffset = 0;
	int32 MouseYOffset = 0;
	CEStaticArray<CEKey, 512> ScanCodeTable{};
	CEStaticArray<uint16, 512> KeyTable{};
};
