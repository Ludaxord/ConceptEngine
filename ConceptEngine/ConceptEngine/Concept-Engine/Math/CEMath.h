#pragma once
#include <type_traits>

namespace ConceptEngine::Math {
	class CEMath {
	public:
		template <typename T>
		static T AlignUp(T Value, T Alignment) {
			static_assert(std::is_integral<T>());
			const T mask = Alignment - 1;
			return ((Value + mask) & (~mask));
		}

	protected:
	private:
	};
}
