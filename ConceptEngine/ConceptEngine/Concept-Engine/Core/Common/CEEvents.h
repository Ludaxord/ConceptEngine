#pragma once

#include "CERef.h"
#include "../Platform/Generic/Window/CEWindow.h"
#include "../Platform/Generic/Input/CEModifierKeyState.h"


namespace ConceptEngine::Core::Common {
	using namespace Platform::Generic::Input;
	using namespace Platform::Generic::Window;

	struct CEKeyPressedEvent {
		CEKeyPressedEvent(CEKey InKey,
		                  bool InIsRepeat,
		                  const CEModifierKeyState& InModifiers) : Key(InKey),
		                                                           IsRepeat(InIsRepeat),
		                                                           Modifiers(InModifiers) {
		}

		CEKey Key;
		bool IsRepeat;
		CEModifierKeyState Modifiers;
	};

	struct CEKeyReleasedEvent {
		CEKeyReleasedEvent(CEKey InKey, const CEModifierKeyState& InModifiers) : Key(InKey), Modifiers(InModifiers) {

		}

		CEKey Key;
		CEModifierKeyState Modifiers;
	};

	struct CEKeyTypedEvent {
		CEKeyTypedEvent(uint32 InCharacter) : Character(InCharacter) {

		}

		const char GetPrintableCharacter() const {
			return static_cast<char>(Character);
		}

		uint32 Character;
	};

	struct CEMouseMovedEvent {
		CEMouseMovedEvent(int32 InX, int32 InY) : x(InX), y(InY) {

		}

		int32 x;
		int32 y;
	};

	struct CEMousePressedEvent {
		CEMousePressedEvent(CEMouseButton InButton, const CEModifierKeyState& InModifiers) : Button(InButton),
			Modifiers(InModifiers) {
		}

		CEMouseButton Button;
		CEModifierKeyState Modifiers;
	};

	struct CEMouseReleasedEvent {
		CEMouseReleasedEvent(CEMouseButton InButton, const CEModifierKeyState& InModifiers) : Button(InButton),
			Modifiers(InModifiers) {

		}

		CEMouseButton Button;
		CEModifierKeyState Modifiers;
	};

	struct CEMouseScrolledEvent {
		CEMouseScrolledEvent(float InHorizontalDelta, float InVerticalDelta): HorizontalDelta(InHorizontalDelta),
		                                                                      VerticalDelta(InVerticalDelta) {

		}

		float HorizontalDelta;
		float VerticalDelta;
	};

	struct CEWindowResizeEvent {
		CEWindowResizeEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow, uint16 InWidth,
		                    uint16 InHeight) : Window(InWindow), Width(InWidth), Height(InHeight) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;
		uint16 Width;
		uint16 Height;
	};

	struct CEWindowFocusChangedEvent {
		CEWindowFocusChangedEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow, bool hasFocus)
			: Window(InWindow)
			  , HasFocus(hasFocus) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;
		bool HasFocus;
	};

	struct CEWindowMovedEvent {
		CEWindowMovedEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow, int16 x, int16 y)
			: Window(InWindow)
			  , Position({x, y}) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;

		struct {
			int16 x;
			int16 y;
		} Position;
	};

	struct CEWindowMouseLeftEvent {
		CEWindowMouseLeftEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow)
			: Window(InWindow) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;
	};

	struct CEWindowMouseEnteredEvent {
		CEWindowMouseEnteredEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow)
			: Window(InWindow) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;
	};

	struct CEWindowClosedEvent {
		CEWindowClosedEvent(const CERef<Platform::Generic::Window::CEWindow>& InWindow)
			: Window(InWindow) {
		}

		CERef<Platform::Generic::Window::CEWindow> Window;
	};
}
