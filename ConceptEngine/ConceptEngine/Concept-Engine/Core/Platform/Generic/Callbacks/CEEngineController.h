#pragma once
#include "CEPlatformCallbacks.h"
#include "../../../Delegates/CEEvent.h"
#include "../../../Common/CEEvents.h"
#include "../CEPlatform.h"

namespace ConceptEngine::Core::Platform::Generic::Callbacks {
	class CEEngineController final : public CEPlatformCallbacks {
	public:
		bool Create();
		bool Release();
		void Exit();

		virtual void
		OnKeyReleased(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) override final;
		virtual void
		OnKeyPressed(Input::CEKey keyCode, bool isRepeat,
		             const Input::CEModifierKeyState& modifierKeyState) override final;
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

		static Window::CEWindow* GetWindow() {
			return Core::Generic::Platform::CEPlatform::GetWindow();
		}
	
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
		DECLARE_EVENT(OnMouseMoveEvent, CEEngineController, const Common::CEMouseMovedEvent&);

		OnMouseMoveEvent OnMouseMoveEvent;

		DECLARE_EVENT(OnMousePressedEvent, CEEngineController, const Common::CEMousePressedEvent&);

		OnMousePressedEvent OnMousePressedEvent;

		DECLARE_EVENT(OnMouseReleasedEvent, CEEngineController, const Common::CEMouseReleasedEvent&);

		OnMouseReleasedEvent OnMouseReleasedEvent;

		DECLARE_EVENT(OnMouseScrolledEvent, CEEngineController, const Common::CEMouseScrolledEvent&);

		OnMouseScrolledEvent OnMouseScrolledEvent;

		/*
		 * Window events
		 */
		DECLARE_EVENT(OnWindowResizedEvent, CEEngineController, const Common::CEWindowResizeEvent&);

		OnWindowResizedEvent OnWindowResizedEvent;

		DECLARE_EVENT(OnWindowMovedEvent, CEEngineController, const Common::CEWindowMovedEvent&);

		OnWindowMovedEvent OnWindowMovedEvent;

		DECLARE_EVENT(OnWindowFocusChangedEvent, CEEngineController, const Common::CEWindowFocusChangedEvent&);

		OnWindowFocusChangedEvent OnWindowFocusChangedEvent;

		DECLARE_EVENT(OnWindowMouseEnteredEvent, CEEngineController, const Common::CEWindowMouseEnteredEvent&);

		OnWindowMouseEnteredEvent OnWindowMouseEnteredEvent;

		DECLARE_EVENT(OnWindowMouseLeftEvent, CEEngineController, const Common::CEWindowMouseLeftEvent&);

		OnWindowMouseLeftEvent OnWindowMouseLeftEvent;

		DECLARE_EVENT(OnWindowClosedEvent, CEEngineController, const Common::CEWindowClosedEvent&);

		OnWindowClosedEvent OnWindowClosedEvent;

		/*
		 * Engine events
		 */
		DECLARE_EVENT(OnApplicationExitEvent, CEEngineController, int32);

		OnApplicationExitEvent OnApplicationExitEvent;


	};

	extern CEEngineController EngineController;
}
