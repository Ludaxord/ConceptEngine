#pragma once

#include "../Core/Common/CETypes.h"

namespace ConceptEngine::Memory {
	class CELinearAllocator {
	public:
		void* Allocate(uint64 sizeInBytes, uint64 alignment);
	};


}

void* operator new(size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void* operator new[](size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void operator delete(void*, ConceptEngine::Memory::CELinearAllocator&);
void operator delete[](void*, ConceptEngine::Memory::CELinearAllocator&);
