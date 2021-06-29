#include "CELinearAllocator.h"

using namespace ConceptEngine::Memory;

CELinearAllocator::CELinearAllocator(uint32 startSize): CurrentArena(nullptr), Arenas() {
	CurrentArena = &Arenas.EmplaceBack(startSize);
}

void* CELinearAllocator::Allocate(uint64 sizeInBytes, uint64 alignment) {
	Assert(CurrentArena != nullptr);
	const uint64 alignedSize = Math::CEMath::AlignUp(sizeInBytes, alignment);
	if (CurrentArena->ReservedSize() > alignedSize) {
		return CurrentArena->Allocate(alignedSize);
	}

	const uint64 currentSize = CurrentArena->GetSizeInBytes();
	uint64 newArenaSize = currentSize + currentSize;
	if (newArenaSize <= alignedSize) {
		newArenaSize = newArenaSize + sizeInBytes;
	}

	CurrentArena = &Arenas.EmplaceBack(newArenaSize);

	Assert(CurrentArena != nullptr);
	return CurrentArena->Allocate(alignedSize);
}

void CELinearAllocator::Reset() {
	Assert(CurrentArena != nullptr);
	CurrentArena->Reset();

	if (Arenas.Size() > 1) {
		Arenas.Front() = ::Move(Arenas.Back());
		Arenas.Resize(1);
		CurrentArena = &Arenas.Front();
	}
}

void* operator new(size_t size, CELinearAllocator& allocator) {
	void* memory = allocator.Allocate(size, 1);
	Assert(memory != nullptr);
	return memory;
}

void* operator new [](size_t size, CELinearAllocator& allocator) {
	void* memory = allocator.Allocate(size, 1);
	Assert(memory != nullptr);
	return memory;
}

void operator delete(void*, CELinearAllocator&) {
}

void operator delete [](void*, CELinearAllocator&) {
}
