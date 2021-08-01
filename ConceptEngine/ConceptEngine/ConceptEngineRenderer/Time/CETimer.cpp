#include "CETimer.h"

#include "Platform/PlatformTime.h"

CETimer::CETimer()
{
    Frequency = PlatformTime::QueryPerformanceFrequency();
    Update();
}

void CETimer::Update()
{
    const uint64 Now = PlatformTime::QueryPerformanceCounter();	
    constexpr uint64 NANOSECONDS = 1000 * 1000 * 1000;
    uint64 Delta       = Now - LastTime;
    uint64 Nanoseconds = (Delta * NANOSECONDS) / Frequency;

    DeltaTime = CETimestamp(Nanoseconds);
    LastTime  = Now;
    TotalTime += DeltaTime;
}