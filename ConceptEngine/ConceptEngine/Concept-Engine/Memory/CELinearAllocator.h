#pragma once
namespace ConceptEngine::Memory {
	class CELinearAllocator {

	};


}

void* operator new(size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void* operator new[](size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void operator delete(void*, ConceptEngine::Memory::CELinearAllocator&);
void operator delete[](void*, ConceptEngine::Memory::CELinearAllocator&);
