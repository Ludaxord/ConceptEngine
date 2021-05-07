#pragma once
#include "CEPlatformCallbacks.h"
#include "../../../Delegates/CEEvent.h"
#include "../../../Common/CEEvents.h"

namespace ConceptEngine::Core::Platform::Generic::Callbacks {
	class CEEngineController final : public CEPlatformCallbacks {
	public:

		bool Create();
		bool Release();
		bool Exit();

		virtual void
		OnKeyReleased(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) override final;
		virtual void
		OnKeyPressed(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) override final;
		virtual void OnKeyTyped(uint32 character) override final;
		virtual void OnMouseMove(int32 x, int32 y) override final;
		virtual void
		OnMouseReleased(Input::CEMouseButton button, const Input::CEModifierKeyState& modifierKeyState) override final;
		virtual void
		OnMousePressed(Input::CEMouseButton button, const Input::CEModifierKeyState& modifierKeyState) override final;
		virtual void OnMouseScrolled(float horizontalDelta, float verticalDelta) override final;
		virtual void
		OnWindowResized(const Common::CERef<Window::CEWindow>& window, uint16 width, uint16 height) override final;
		virtual void OnWindowMoved(const Common::CERef<Window::CEWindow>& window, uint16 x, uint16 y) override final;
		virtual void OnWindowFocusChanged(const Common::CERef<Window::CEWindow>& window, bool hasFocus) override final;
		virtual void OnWindowMouseLeft(const Common::CERef<Window::CEWindow>& window) override final;
		virtual void OnWindowMouseEntered(const Common::CERef<Window::CEWindow>& window) override final;
		virtual void OnWindowClosed(const Common::CERef<Window::CEWindow>& window) override final;
		virtual void OnApplicationExit(int32 exitCode) override final;

	public:
		/*
		 * Key Events
		 */
		DECLARE_EVENT(OnKeyReleasedEvent, CEEngineController, const Common::CEKeyReleasedEvent&);

		OnKeyReleasedEvent OnKeyReleasedEvent;
		DECLARE_EVENT(OnKeyPressedEvent, CEEngineController, const Common::CEKeyPressedEvent&);

		OnKeyPressedEvent OnKeyPressedEvent;
		DECLARE_EVENT(OnKeyTypedEvent, CEEngineController, const Common::CEKeyTypedEvent&);

		OnKeyTypedEvent OnKeyTypedEvent;

		/*
		 * Mouse events
		 */

		/*
		 * Window events
		 */

		/*
		 * Engine events
		 */

	public:
		bool IsRunning = false;
	};

	extern CEEngineController EngineController;
}
