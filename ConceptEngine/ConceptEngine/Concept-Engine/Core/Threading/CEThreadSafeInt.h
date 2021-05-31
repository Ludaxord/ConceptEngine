#pragma once
#include "CEAtomic.h"
#include "../../Math/CEMath.h"

namespace ConceptEngine::Core::Threading {
	template <typename T>
	class CEThreadSafeInt {
	public:
		CEThreadSafeInt() noexcept: Value(0) {

		}

		CEThreadSafeInt(T value) noexcept : Value(value) {

		}

		CEThreadSafeInt(const CEThreadSafeInt&) = delete;

		T Increment() noexcept;

		T Decrement() noexcept;

		T Add(T rhs) noexcept;

		T Sub(T rhs) noexcept;

		T Mul(T rhs) noexcept;

		T Div(T rhs) noexcept;

		T Load() noexcept;

		void Store(T value) noexcept;

		CEThreadSafeInt& operator=(const CEThreadSafeInt&) = delete;

		T operator=(T rhs) noexcept {
			return Store(rhs);
		}

		T operator++(int32) noexcept {
			return Increment();
		}

		T operator--(int32) noexcept {
			return Decrement();
		}

		T operator+=(T rhs) noexcept {
			return Add(rhs);
		}

		T operator-=(T rhs) noexcept {
			return Sub(rhs);
		}

		T operator*=(T rhs) noexcept {
			return Mul(rhs);
		}

		T operator/=(T rhs) noexcept {
			return Div(rhs);
		}

	protected:
	private:
		volatile T Value;
	};

	typedef CEThreadSafeInt<int32> CEThreadSafeInt32;
	typedef CEThreadSafeInt<int64> CEThreadSafeInt64;

	template <>
	inline int32 CEThreadSafeInt<int32>::Increment() noexcept {
		return CEAtomic::InterlockedIncrement(&Value);
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Decrement() noexcept {
		return CEAtomic::InterlockedDecrement(&Value);
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Add(int32 rhs) noexcept {
		return CEAtomic::InterlockedAdd(&Value, rhs);
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Sub(int32 rhs) noexcept {
		return CEAtomic::InterlockedSub(&Value, rhs);
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Mul(int32 rhs) noexcept {
		return 0;
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Div(int32 rhs) noexcept {
		return 0;
	}

	template <>
	inline int32 CEThreadSafeInt<int32>::Load() noexcept {
		CEAtomic::InterlockedCompareExchange(&Value, 0, 0);
		return Value;
	}

	template <>
	inline void CEThreadSafeInt<int32>::Store(int32 value) noexcept {
		CEAtomic::InterlockedExchange(&Value, value);
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Increment() noexcept {
		return CEAtomic::InterlockedIncrement(&Value);
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Decrement() noexcept {
		return CEAtomic::InterlockedDecrement(&Value);
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Add(int64 rhs) noexcept {
		return CEAtomic::InterlockedAdd(&Value, rhs);
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Sub(int64 rhs) noexcept {
		return CEAtomic::InterlockedSub(&Value, rhs);
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Mul(int64 rhs) noexcept {
		return 0;
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Div(int64 rhs) noexcept {
		return 0;
	}

	template <>
	inline int64 CEThreadSafeInt<int64>::Load() noexcept {
		CEAtomic::InterlockedCompareExchange(&Value, 0, 0);
		return Value;
	}

	template <>
	inline void CEThreadSafeInt<int64>::Store(int64 value) noexcept {
		CEAtomic::InterlockedExchange(&Value, value);
	}


}
