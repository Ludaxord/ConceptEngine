#pragma once

#include "../../../Common/CETypes.h"

namespace ConceptEngine::Core::Platform::Generic::Time {
	class CEGenericTime {
	public:
		static uint64 QueryPerformanceCounter() {
			return 0;
		}

		static uint64 QueryPerformanceFrequency() {
			return 1;
		}
	};
}
