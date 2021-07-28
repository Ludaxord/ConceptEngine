#pragma once
#include "../../Time/Timer.h"

class EngineLoop
{
public:
    static bool Init();
  
    static void Tick(CETimestamp Deltatime);

    static void Run();
    
    static bool Release();
};