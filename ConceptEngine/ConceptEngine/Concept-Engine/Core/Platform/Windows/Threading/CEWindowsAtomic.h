#pragma once

#include <Windows.h>

#include "../../../Threading/CEGenericAtomic.h"

namespace ConceptEngine::Core::Platform::Windows::Threading {
	class CEWindowsAtomic : public Core::Threading::CEGenericAtomic {
	public:
		inline static int32 InterlockedIncrement(volatile int32* dest) {
			return _InterlockedIncrement((LONG*)dest);
		}

		inline static int64 InterlockedIncrement(volatile int64* dest) {
			return _InterlockedIncrement64(dest);
		}

		inline static int32 InterlockedDecrement(volatile int32* dest) {
			return _InterlockedDecrement((LONG*)dest);
		}

		inline static int64 InterlockedDecrement(volatile int64* dest) {
			return _InterlockedDecrement64(dest);
		}

		inline static int32 InterlockedAdd(volatile int32* dest, int32 value) {
			return _InlineInterlockedAdd((LONG*)dest, value);
		}

		inline static int64 InterlockedAdd(volatile int64* dest, int64 value) {
			return _InlineInterlockedAdd64(dest, value);
		}

		inline static int32 InterlockedSub(volatile int32* dest, int32 value) {
			return _InlineInterlockedAdd((LONG*)dest, -value);
		}

		inline static int64 InterlockedSub(volatile int64* dest, int64 value) {
			return _InlineInterlockedAdd64(dest, -value);
		}

		inline static int32 InterlockedMul(volatile int32* dest, int32 value) {
			return _InlineInterlockedAdd((LONG*)dest, -value);
		}

		inline static int64 InterlockedMul(volatile int64* dest, int64 value) {
			return _InlineInterlockedAdd64(dest, -value);
		}

		inline static int32 InterlockedCompareExchange(volatile int32* dest, int32 exchange, int32 comparand) {
			return _InterlockedCompareExchange((LONG*)dest, exchange, comparand);
		}

		inline static int64 InterlockedCompareExchange(volatile int64* dest, int64 exchange, int64 comparand) {
			return _InterlockedCompareExchange64(dest, exchange, comparand);
		}

		inline static int32 InterlockedExchange(volatile int32* dest, int32 exchange) {
			return _InterlockedExchange((LONG*)dest, exchange);
		}

		inline static int64 InterlockedExchange(volatile int64* dest, int64 exchange) {
			return _InterlockedExchange64(dest, exchange);
		}
	};
}
