#include "CEMemory.h"

#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <crtdbg.h>
#endif

#pragma warning(disable: 4996)

using namespace ConceptEngine::Memory;

void* CEMemory::Malloc(uint64 size) {
	return ::malloc(size);
}

void CEMemory::Free(void* Ptr) {
	::free(Ptr);
}

void* CEMemory::Memset(void* destination, uint8 value, uint64 size) {
	return ::memset(destination, static_cast<int>(value), size);
}

void* CEMemory::Memzero(void* destination, uint64 size) {
	return ::memset(destination, 0, size);
}

void* CEMemory::Memcpy(void* destination, const void* source, uint64 size) {
	return ::memcpy(destination, source, size);
}

void* CEMemory::Memmove(void* destination, const void* source, uint64 size) {
	return ::memmove(destination, source, size);
}

char* CEMemory::Strcpy(char* destination, const char* source) {
	//TODO: replace unsafe function
	return ::strcpy(destination, source);
}
