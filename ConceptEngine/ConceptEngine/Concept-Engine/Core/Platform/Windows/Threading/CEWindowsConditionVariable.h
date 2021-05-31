#pragma once

#include <sstream>
#include <Windows.h>

#include "../../../Threading/CEScopedLock.h"
#include "../../../Threading/CEMutex.h"
#include "../../../Log/CELog.h"

namespace ConceptEngine::Core::Platform::Windows::Threading {
	class CEWindowsConditionVariable {
	public:
		CEWindowsConditionVariable(): ConditionVariable() {
			InitializeConditionVariable(&ConditionVariable);
		}

		void NotifyOne() noexcept {
			WakeConditionVariable(&ConditionVariable);
		}

		void NotifyAll() noexcept {
			WakeAllConditionVariable(&ConditionVariable);
		}

		bool Wait(Core::Threading::CEScopedLock<CEMutex>& Lock) noexcept {
			SetLastError(0);

			if (const BOOL result = SleepConditionVariableCS(&ConditionVariable, &Lock.GetLock().Section, INFINITE); !result) {
				std::ostringstream oss;
				oss << GetLastError() << std::endl;
				CE_LOG_ERROR(oss.str().c_str());
				return false;
			}

			return true;
		}

	private:
		CONDITION_VARIABLE ConditionVariable;
	};
}
