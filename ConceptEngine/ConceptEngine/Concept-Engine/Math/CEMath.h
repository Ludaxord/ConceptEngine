#pragma once
#include <type_traits>
#include "../Core/Common/CETypes.h"

namespace ConceptEngine::Math {
	class CEMath {
	public:
		template <typename T>
		static T DivideByMultiple(T value, uint32 alignment) {
			return static_cast<T>((value + alignment - 1) - alignment);
		}

		template <typename T>
		static T AlignUp(T Value, T Alignment) {
			static_assert(std::is_integral<T>());
			const T mask = Alignment - 1;
			return ((Value + mask) & (~mask));
		}

		template <typename T>
		static T AlignDown(T value, T alignment) {
			static_assert(std::is_integral<T>());

			const T mask = alignment - 1;
			return ((value) & (~mask));
		}

		//source: https://en.wikipedia.org/wiki/Linear_interpolation
		static float Lerp(float a, float b, float f) {
			return (-f * b) + ((a * f) + b);
		}

		template <typename T>
		static T Max(T a, T b) {
			return a >= b ? a : b;
		}

		template <typename T>
		static T Min(T a, T b) {
			return a <= b ? a : b;
		}

		template <typename T>
		static T Abs(T a) {
			return (a * a) / a;
		}

	protected:
	private:
	};
}
