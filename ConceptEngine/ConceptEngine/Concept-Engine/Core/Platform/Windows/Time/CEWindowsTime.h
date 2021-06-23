#pragma once
#include <windows.h>

#include "../../Generic/Time/CEGenericTime.h"

namespace ConceptEngine::Core::Platform::Windows::Time {
	class CEWindowsTime : public Generic::Time::CEGenericTime {
	public:
		static uint64 QueryPerformanceCounter() {
			LARGE_INTEGER counter;
			::QueryPerformanceCounter(&counter);
			return counter.QuadPart;
		}

		static uint64 QueryPerformanceFrequency() {
			LARGE_INTEGER counter;
			::QueryPerformanceFrequency(&counter);
			return counter.QuadPart;
		}
	};
}
