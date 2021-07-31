#include "CEMemory.h"

#include <cstdlib>
#include <cstring>
#ifdef _WIN32
    #include <crtdbg.h>
#endif

void* CEMemory::Malloc(uint64 Size)
{
    return ::malloc(Size);
}

void CEMemory::Free(void* Ptr)
{
    ::free(Ptr);
}

char* CEMemory::Strcpy(char* Destination, const char* Source)
{
    return ::strcpy(Destination, Source);
}

void* CEMemory::Memset(void* Destination, uint8 Value, uint64 Size)
{
    return ::memset(Destination, static_cast<int>(Value), Size);
}

void* CEMemory::Memzero(void* Destination, uint64 Size)
{
    return ::memset(Destination, 0, Size);
}

void* CEMemory::Memcpy(void* Destination, const void* Source, uint64 Size)
{
    return ::memcpy(Destination, Source, Size);
}

void* CEMemory::Memmove(void* Destination, const void* Source, uint64 Size)
{
    return ::memmove(Destination, Source, Size);
}
