#pragma once
#include "../Core/Application/Platform/CEPlatformMisc.h"

#ifdef OutputDebugString
    #undef OutputDebugString
#endif

class CEDebug
{
public:
    FORCEINLINE static void DebugBreak()
    {
        CEPlatformMisc::DebugBreak();
    }

    FORCEINLINE static void OutputDebugString(const std::string& Message)
    {
        CEPlatformMisc::OutputDebugString(Message);
    }

    FORCEINLINE static bool IsDebuggerPresent()
    {
        return CEPlatformMisc::IsDebuggerPresent();
    }
};