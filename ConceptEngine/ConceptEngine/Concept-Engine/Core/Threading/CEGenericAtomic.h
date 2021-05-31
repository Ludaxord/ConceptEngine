#pragma once

#include "../../Math/CEMath.h"

#ifdef InterlockedAdd
#undef InterlockedAdd
#endif

#ifdef InterlockedIncrement
#undef InterlockedIncrement
#endif

#ifdef InterlockedDecrement
#undef InterlockedDecrement
#endif

#ifdef InterlockedCompareExchange
#undef InterlockedCompareExchange
#endif

#ifdef InterlockedExchange
#undef InterlockedExchange
#endif

namespace ConceptEngine::Core::Threading {
	class CEGenericAtomic {
	public:
		inline static int32 InterlockedIncrement(volatile int32* dest) {
			return 0;
		}

		inline static int64 InterlockedIncrement(volatile int64* dest) {
			return 0;
		}

		inline static int32 InterlockedDecrement(volatile int32* dest) {
			return 0;
		}

		inline static int64 InterlockedDecrement(volatile int64* dest) {
			return 0;
		}

		inline static int32 InterlockedAdd(volatile int32* dest, int32 value) {
			return 0;
		}

		inline static int64 InterlockedAdd(volatile int64* dest, int64 value) {
			return 0;
		}

		inline static int32 InterlockedSub(volatile int32* dest, int32 value) {
			return 0;
		}

		inline static int64 InterlockedSub(volatile int64* dest, int64 value) {
			return 0;
		}

		inline static int32 InterlockedCompareExchange(volatile int32* dest, int32 exchange, int32 comparand) {
			return 0;
		}

		inline static int64 InterlockedCompareExchange(volatile int64* dest, int64 exchange, int64 comparand) {
			return 0;
		}

		inline static int32 InterlockedExchange(volatile int32* dest, int32 exchange) {
			return 0;
		}

		inline static int64 InterlockedExchange(volatile int64* dest, int64 exchange) {
			return 0;
		}
	};
}
