#include "New.h"
#include "Memory.h"

void* operator new(size_t Size)
{
    return CEMemory::Malloc(Size);
}

void* operator new[](size_t Size)
{
    return CEMemory::Malloc(Size);
}

void* operator new(size_t Size, const std::nothrow_t&) noexcept
{
    return CEMemory::Malloc(Size);
}

void* operator new[](size_t Size, const std::nothrow_t&) noexcept
{
    return CEMemory::Malloc(Size);
}

void operator delete(void* Ptr) noexcept
{
    CEMemory::Free(Ptr);
}

void operator delete[](void* Ptr) noexcept
{
    CEMemory::Free(Ptr);
}

void operator delete(void* Ptr, size_t) noexcept
{
    CEMemory::Free(Ptr);
}

void operator delete[](void* Ptr, size_t) noexcept
{
    CEMemory::Free(Ptr);
}