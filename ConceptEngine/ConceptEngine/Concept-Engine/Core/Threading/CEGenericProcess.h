#pragma once
#include "CEThread.h"
#include "../../Math/CEMath.h"
#include "../../Time/CETimestamp.h"

namespace ConceptEngine::Core::Threading {
	class CEGenericProcess {
	public:
		inline static uint32 GetNumProcessors() {
			return 1;
		}

		inline static ThreadID GetThreadID() {
			return INVALID_THREAD_ID;
		}

		inline static void Sleep(Time::CETimestamp time) {
			(void)(time);
		}
	};
}
