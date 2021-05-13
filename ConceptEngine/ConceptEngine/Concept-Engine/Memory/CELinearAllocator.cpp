#include "CELinearAllocator.h"

void* operator new(size_t size, ConceptEngine::Memory::CELinearAllocator& allocator) {
	void* memory = allocator.Allocate(size, 1);
	Assert(memory != nullptr);
	return memory;
}

void* operator new [](size_t size, ConceptEngine::Memory::CELinearAllocator& allocator) {
	void* memory = allocator.Allocate(size, 1);
	Assert(memory != nullptr);
	return memory;
}

void operator delete(void*, ConceptEngine::Memory::CELinearAllocator&) {
}

void operator delete [](void*, ConceptEngine::Memory::CELinearAllocator&) {
}
