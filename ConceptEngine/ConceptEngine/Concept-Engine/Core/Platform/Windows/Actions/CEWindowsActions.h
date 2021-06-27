#pragma once
#include <Windows.h>

#include "../../Generic/Actions/CEActions.h"
#include "../../../Log/CELog.h"

#ifdef MessageBox
#undef MessageBox
#endif

#ifdef OutputDebugString
#undef OutputDebugString
#endif

namespace ConceptEngine::Core::Platform::Windows::Actions {
	class CEWindowsActions : public Generic::Actions::CEActions {
	public:
		static void MessageBox(const std::string& title, const std::string& message) {
			MessageBoxA(0, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
		}

		static void RequestExit(int32_t exitCode) {
			PostQuitMessage(exitCode);
		}

		static void DebugBreak() { 
			__debugbreak();
		}

		static void OutputDebugString(const std::string& message) {
			OutputDebugStringA(message.c_str());
		}

		static bool IsDebuggerPresent() {
			return IsDebuggerPresent();
		}

		template <typename T>
		static T GetTypedProcAddress(HMODULE hModule, LPCSTR lpProcName) {
			T Func = reinterpret_cast<T>(GetProcAddress(hModule, lpProcName));
			if (!Func) {
				const char* procName = lpProcName;
				CE_LOG_ERROR("Failed to load: " + std::string(procName));
			}

			return Func;
		}
	};
}
