#pragma once

#include "../Input/CEKey.h"
#include "../../../Common/CERef.h"

namespace ConceptEngine::Core::Platform::Generic { namespace Input {
		struct CEModifierKeyState;
	}

	namespace Window {
		class CEWindow;
	}


	namespace Callbacks {
		class CEPlatformCallbacks {
		public:
			virtual ~CEPlatformCallbacks() = default;

			virtual void OnKeyReleased(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) {

			}

			virtual void OnKeyPressed(Input::CEKey keyCode, const Input::CEModifierKeyState& modifierKeyState) {

			}

			virtual void OnKeyTyped(uint32 character) {
			}

			virtual void OnMouseMove(int32 x, int32 y) {

			}

			virtual void OnMouseReleased(Input::CEMouseButton button,
			                             const Input::CEModifierKeyState& modifierKeyState) {

			}

			virtual void OnMousePressed(Input::CEMouseButton button,
			                            const Input::CEModifierKeyState& modifierKeyState) {

			}

			virtual void OnMouseScrolled(float horizontalDelta, float verticalDelta) {

			}

			virtual void OnWindowResized(const Common::CERef<Window::CEWindow>& window, uint16 width, uint16 height) {

			}

			virtual void OnWindowMoved(const Common::CERef<Window::CEWindow>& window, uint16 x, uint16 y) {

			}

			virtual void OnWindowFocusChanged(const Common::CERef<Window::CEWindow>& window, bool hasFocus) {

			}

			virtual void OnWindowMouseLeft(const Common::CERef<Window::CEWindow>& window) {

			}

			virtual void OnWindowMouseEntered(const Common::CERef<Window::CEWindow>& window) {

			}

			virtual void OnWindowClosed(const Common::CERef<Window::CEWindow>& window) {

			}

			virtual void OnApplicationExit(int32 exitCode) {

			}

		protected:
		private:
		};
	}}
