#include "CELinearAllocator.h"

void* operator new(size_t size, ConceptEngine::Memory::CELinearAllocator& allocator) {
	return nullptr;
}

void* operator new [](size_t size, ConceptEngine::Memory::CELinearAllocator& allocator) {
	return nullptr;
}

void operator delete(void*, ConceptEngine::Memory::CELinearAllocator&) {
}

void operator delete [](void*, ConceptEngine::Memory::CELinearAllocator&) {
}
