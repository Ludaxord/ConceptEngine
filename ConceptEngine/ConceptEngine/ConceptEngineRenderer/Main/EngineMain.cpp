#include "EngineMain.h"

#include "../Core/Engine/EngineLoop.h"
#include "../Core/Application/Application.h"
#include "../Core/Application/Platform/CEPlatformMisc.h"

int32 EngineMain()
{
    if (!EngineLoop::Init())
    {
        CEPlatformMisc::MessageBox("ERROR", "EngineLoop::Init Failed");
        return -1;
    }

    EngineLoop::Run();

    if (!EngineLoop::Release())
    {
        CEPlatformMisc::MessageBox("ERROR", "EngineLoop::Release Failed");
        return -1;
    }

    return 0;
}
