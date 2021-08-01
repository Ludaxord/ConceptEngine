#pragma once
#include "../../Time/CETimer.h"

class EngineLoop
{
public:
    static bool Init();
  
    static void Tick(CETimestamp Deltatime);

    static void Run();
    
    static bool Release();
};