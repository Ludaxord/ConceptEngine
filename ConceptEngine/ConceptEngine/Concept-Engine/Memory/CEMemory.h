#pragma once

#include "../Core/Common/CETypes.h"

namespace ConceptEngine::Memory {
	class CEMemory {
	public:
		static void* Malloc(uint64 size);
		static void Free(void* Ptr);

		template <typename T>
		static T* Malloc(uint32 count) {
			return reinterpret_cast<T*>(Malloc(sizeof(T) * count));
		}

		static void* Memset(void* destination, uint8 value, uint64 size);
		static void* Memzero(void* destination, uint64 size);

		template <typename T>
		static T* Memzero(T* destination) {
			return reinterpret_cast<T*>(Memzero(destination, sizeof(T)));
		}

		static void* Memcpy(void* destination, const void* source, uint64 size);

		template <typename T>
		static T* Memcpy(T* destination, const T* source) {
			return reinterpret_cast<T*>(Memcpy(destination, source, sizeof(T)));
		}

		static void* Memmove(void* destination, const void* source, uint64 size);
		static char* Strcpy(char* destination, const char* source);

	protected:
	private:
	};
}
