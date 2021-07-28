#pragma once
#include "CETimestamp.h"

class Timer
{
public:
    Timer();

    /*
    * Measures the deltatime between this and the latest call to Clock::Tick. It also updates the totalTime that the clock
    * has been active. This is the time between the last call to Clock::Reset and this call to Clock::Tick
    */
    void Tick();

    void Reset()
    {
        DeltaTime = CETimestamp(0);
        TotalTime = CETimestamp(0);
    }

    const CETimestamp& GetDeltaTime() const { return DeltaTime; }
    const CETimestamp& GetTotalTime() const { return TotalTime; }

private:
    CETimestamp TotalTime = CETimestamp(0);
    CETimestamp DeltaTime = CETimestamp(0);
    uint64 LastTime  = 0;
    uint64 Frequency = 0;
};