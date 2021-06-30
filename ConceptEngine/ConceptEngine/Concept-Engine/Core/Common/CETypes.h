#pragma once

#include <assert.h>

#define DEBUG_BUILD 1

// Assert
#ifdef DEBUG_BUILD
    #define ENABLE_ASSERTS 1
#endif

#ifndef Assert
#if ENABLE_ASSERTS
    #define Assert(Condition) assert(Condition)
#else
    #define Assert(Condition) (void)(0)
#endif
#endif

#define _PREPROCESS_CONCAT(x, y) x##y
#define PREPROCESS_CONCAT(x, y) _PREPROCESS_CONCAT(x, y)

// Bit-Mask helpers
#define BIT(Bit)  (1 << Bit)
#define FLAG(Bit) BIT(Bit)

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;
