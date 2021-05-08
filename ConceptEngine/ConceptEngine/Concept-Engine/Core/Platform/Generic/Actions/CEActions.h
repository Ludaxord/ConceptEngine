#pragma once
#include <string>

namespace ConceptEngine::Core::Platform {
	namespace Generic::Actions {
		class CEActions {
		public:
			static void MessageBox(const std::string& title, const std::string& message) {

			}

			static void RequestExit(int32_t exitCode) {

			}

			static void DebugBreak() {

			}

			static void OutputDebugString(const std::string& message) {
			}

			static bool IsDebuggerPresent() {
				return false;
			}
		};
	}}
