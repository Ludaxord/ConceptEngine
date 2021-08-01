#pragma once
#include "Core/Application/Events.h"
#include "Core/Application/Platform/CEPlatformCallbacks.h"
#include "../../Core/Delegates/Event.h"

class CEEngineController final : public CEPlatformCallbacks {
public:
	bool Create();
	bool Release();

	void Exit();
	~CEEngineController() override;
	void OnKeyReleased(CEKey KeyCode, const CEModifierKeyState& ModierKeyState) override;
	void OnKeyPressed(CEKey KeyCode, bool IsRepeat, const CEModifierKeyState& ModierKeyState) override;
	void OnKeyTyped(uint32 Character) override;
	void OnMouseMove(int32 x, int32 y) override;
	void OnMouseReleased(CEMouseButton Button, const CEModifierKeyState& ModierKeyState) override;
	void OnMousePressed(CEMouseButton Button, const CEModifierKeyState& ModierKeyState) override;
	void OnMouseScrolled(float HorizontalDelta, float VerticalDelta) override;
	void OnWindowResized(const CERef<CEWindow>& Window, uint16 Width, uint16 Height) override;
	void OnWindowMoved(const CERef<CEWindow>& Window, int16 x, int16 y) override;
	void OnWindowFocusChanged(const CERef<CEWindow>& Window, bool HasFocus) override;
	void OnWindowMouseLeft(const CERef<CEWindow>& Window) override;
	void OnWindowMouseEntered(const CERef<CEWindow>& Window) override;
	void OnWindowClosed(const CERef<CEWindow>& Window) override;
	void OnApplicationExit(int32 ExitCode) override;


public:
	// Key events
	DECLARE_EVENT(OnKeyReleasedEvent, CEEngineController, const KeyReleasedEvent&);

	OnKeyReleasedEvent OnKeyReleasedEvent;
	DECLARE_EVENT(OnKeyPressedEvent, CEEngineController, const KeyPressedEvent&);

	OnKeyPressedEvent OnKeyPressedEvent;
	DECLARE_EVENT(OnKeyTypedEvent, CEEngineController, const KeyTypedEvent&);

	OnKeyTypedEvent OnKeyTypedEvent;

	// Mouse events
	DECLARE_EVENT(OnMouseMoveEvent, CEEngineController, const MouseMovedEvent&);

	OnMouseMoveEvent OnMouseMoveEvent;
	DECLARE_EVENT(OnMousePressedEvent, CEEngineController, const MousePressedEvent&);

	OnMousePressedEvent OnMousePressedEvent;
	DECLARE_EVENT(OnMouseReleasedEvent, CEEngineController, const MouseReleasedEvent&);

	OnMouseReleasedEvent OnMouseReleasedEvent;
	DECLARE_EVENT(OnMouseScrolledEvent, CEEngineController, const MouseScrolledEvent&);

	OnMouseScrolledEvent OnMouseScrolledEvent;

	// Window Events
	DECLARE_EVENT(OnWindowResizedEvent, CEEngineController, const WindowResizeEvent&);

	OnWindowResizedEvent OnWindowResizedEvent;
	DECLARE_EVENT(OnWindowMovedEvent, CEEngineController, const WindowMovedEvent&);

	OnWindowMovedEvent OnWindowMovedEvent;
	DECLARE_EVENT(OnWindowFocusChangedEvent, CEEngineController, const WindowFocusChangedEvent&);

	OnWindowFocusChangedEvent OnWindowFocusChangedEvent;
	DECLARE_EVENT(OnWindowMouseEnteredEvent, CEEngineController, const WindowMouseEnteredEvent&);

	OnWindowMouseEnteredEvent OnWindowMouseEnteredEvent;
	DECLARE_EVENT(OnWindowMouseLeftEvent, CEEngineController, const WindowMouseLeftEvent&);

	OnWindowMouseLeftEvent OnWindowMouseLeftEvent;
	DECLARE_EVENT(OnWindowClosedEvent, CEEngineController, const WindowClosedEvent&);

	OnWindowClosedEvent OnWindowClosedEvent;

	// Engine Events
	DECLARE_EVENT(OnApplicationExitEvent, CEEngineController, int32);

	OnApplicationExitEvent OnApplicationExitEvent;

	bool bIsRunning = false;
};

extern CEEngineController GEngineController;
