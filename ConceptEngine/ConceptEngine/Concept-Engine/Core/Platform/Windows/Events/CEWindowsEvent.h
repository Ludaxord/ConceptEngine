#pragma once

#include <Windows.h>

#include "../../../Common/CETypes.h"
#include "../../Generic/Events/CEEvent.h"

namespace ConceptEngine::Core::Platform::Windows::Events {
	struct CEWindowsEvent : Generic::Events::CEEvent {
		CEWindowsEvent(HWND window, uint32 message, WPARAM wParam, LPARAM lParam): Window(window), Message(message),
			wParam(wParam), lParam(lParam) {

		}

		HWND Window;
		uint32 Message;
		WPARAM wParam;
		LPARAM lParam;
	};
}
