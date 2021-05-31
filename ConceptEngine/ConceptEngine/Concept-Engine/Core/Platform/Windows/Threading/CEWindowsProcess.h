#pragma once

#include "../../../Threading/CEGenericProcess.h"

#include "../../../../Memory/CEMemory.h"

namespace ConceptEngine::Core::Platform::Windows::Threading {
	class CEWindowsProcess : public Core::Threading::CEGenericProcess {
	public:
		inline static uint32 GetNumProcessors() {
			SYSTEM_INFO systemInfo;
			ConceptEngine::Memory::CEMemory::Memzero(&systemInfo);

			GetSystemInfo(&systemInfo);

			return systemInfo.dwNumberOfProcessors;
		}

		inline static Core::Threading::ThreadID GetThreadID() {
			DWORD currentID = GetCurrentThreadId();
			return static_cast<Core::Threading::ThreadID>(currentID);
		}

		inline static void Sleep(Time::CETimestamp time) {
			DWORD milliSeconds = (DWORD)time.AsMilliSeconds();
			::Sleep(milliSeconds);
		}
	};
}
