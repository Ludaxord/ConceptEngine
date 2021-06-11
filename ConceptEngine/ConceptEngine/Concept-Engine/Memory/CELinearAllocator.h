#pragma once

#include "CEMemory.h"
#include "../Core/Common/CETypes.h"
#include "../Core/Containers/CEArray.h"
#include "../Math/CEMath.h"

namespace ConceptEngine::Memory {
	struct CEMemoryArena {
		CEMemoryArena(uint64 sizeInBytes) : Mem(nullptr), Offset(0), SizeInBytes(sizeInBytes) {
			Mem = reinterpret_cast<uint8*>(CEMemory::Malloc(SizeInBytes));
		}

		CEMemoryArena(const CEMemoryArena& ma) = delete;

		CEMemoryArena(CEMemoryArena&& ma): Mem(ma.Mem), Offset(ma.Offset), SizeInBytes(ma.SizeInBytes) {
			ma.Mem = nullptr;
			ma.Offset = 0;
			ma.SizeInBytes = 0;
		}

		~CEMemoryArena() {
			CEMemory::Free(Mem);
		}

		void* Allocate(uint64 sizeInBytes) {
			Assert(ReservedSize() >= sizeInBytes);
			void* allocated = reinterpret_cast<void*>(Mem + Offset);
			Offset += sizeInBytes;
			return allocated;
		}

		uint64 ReservedSize() {
			return SizeInBytes - Offset;
		}

		void Reset() {
			Offset = 0;
		}

		uint64 GetSizeInBytes() const {
			return SizeInBytes;
		}

		CEMemoryArena& operator=(const CEMemoryArena& ma) = delete;

		CEMemoryArena& operator=(CEMemoryArena&& ma) {
			if (Mem) {
				CEMemory::Free(Mem);
			}

			Mem = ma.Mem;
			Offset = ma.Offset;
			SizeInBytes = ma.SizeInBytes;

			ma.Mem = nullptr;
			ma.Offset = 0;
			ma.SizeInBytes = 0;

			return *this;
		}

		uint8* Mem;
		uint64 Offset;
		uint64 SizeInBytes;
	};

	class CELinearAllocator {
	public:
		CELinearAllocator(uint32 startSize = 4096);
		~CELinearAllocator() = default;

		void* Allocate(uint64 sizeInBytes, uint64 alignment);

		void Reset();

		template <typename T>
		void* Allocate() {
			return Allocate(sizeof(T), alignof(T));
		}

		uint8* AllocateBytes(uint64 sizeInBytes, uint64 alignment) {
			return reinterpret_cast<uint8*>(Allocate(sizeInBytes, alignment));
		}

	private:
		CEMemoryArena* CurrentArena;
		CEArray<CEMemoryArena> Arenas;
	};


}

void* operator new(size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void* operator new[](size_t size, ConceptEngine::Memory::CELinearAllocator& allocator);
void operator delete(void*, ConceptEngine::Memory::CELinearAllocator&);
void operator delete[](void*, ConceptEngine::Memory::CELinearAllocator&);
