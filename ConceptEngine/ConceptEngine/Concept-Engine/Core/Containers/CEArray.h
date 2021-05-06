#pragma once
#include "CEAllocator.h"
#include "CEIterator.h"

namespace ConceptEngine::Core::Containers {
	template <typename T, typename TAllocator = CEAllocator>
	class CEArray {
	public:
		typedef T* Iterator;
		typedef const T* ConstIterator;
		typedef CEReverseIterator<T> ReverseIterator;
		typedef CEReverseIterator<const T> ConstReverseIterator;
		typedef uint32 SizeType;

		CEArray() noexcept : Array(nullptr), ArraySize(0), ArrayCapacity(0), Allocator() {

		}

		explicit CEArray(SizeType Size) noexcept : Array(nullptr), ArraySize(0), ArrayCapacity(0), Allocator() {
			// InternalConstruct(Size);
		}

	protected:
	private:
		T* Array;
		SizeType ArraySize;
		SizeType ArrayCapacity;
		TAllocator Allocator;
	};
}
