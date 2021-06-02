#pragma once

#include "../../Core/Platform/CEPlatformActions.h"

namespace ConceptEngine::Core::Debug {
	class CEDebug {
	public:
		static void DebugBreak() {
			CEPlatformActions::DebugBreak();
		}

		static void OutputDebugString(const std::string& message) {
			CEPlatformActions::OutputDebugString(message);
		}

		static bool IsDebuggerPresent() {
			return CEPlatformActions::IsDebuggerPresent();
		}
	};
}
