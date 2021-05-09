#pragma once
#include "CEKey.h"

namespace ConceptEngine::Core::Platform::Generic::Input {
	struct CEModifierKeyState {
	public:
		CEModifierKeyState() = default;

		CEModifierKeyState(uint32 InModifierMask): ModifierMask(InModifierMask) {

		}

		bool IsCtrlDown() const {
			return (ModifierMask & ModifierFlag_Ctrl);
		}

		bool IsAltDown() const {
			return (ModifierMask & ModifierFlag_Alt);
		}

		bool IsShiftDown() const {
			return (ModifierMask & ModifierFlag_Shift);
		}

		bool IsCapsLockDown() const {
			return (ModifierMask & ModifierFlag_CapsLock);
		}

		bool IsSuperKeyDown() const {
			return (ModifierMask & ModifierFlag_Super);
		}

		bool IsNumPadDown() const {
			return (ModifierMask & ModifierFlag_NumLock);
		}

		uint32 ModifierMask = 0;
	};
}
