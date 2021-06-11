#pragma once

#include <cstdlib>

#include "../Common/CETypes.h"

struct CEAllocator {
	void* Allocate(uint32 Size) {
		return malloc(Size);
	}

	void Free(void* Ptr) {
		free(Ptr);
	}
};
