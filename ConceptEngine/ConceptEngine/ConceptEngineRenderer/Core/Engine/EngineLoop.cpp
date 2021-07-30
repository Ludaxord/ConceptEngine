#include "Engine.h"
#include "EngineLoop.h"

#include "../../Core/Engine/EngineGlobals.h"
#include "../../Core/Application/Application.h"
#include "../../Core/Application/Generic/GenericOutputConsole.h"
#include "../../Core/Application/Platform/Platform.h"
#include "../../Core/Application/Platform/CEPlatformMisc.h"
#include "../../Core/Input/CEInputManager.h"
#include "../../Core/Threading/CETaskManager.h"

#include "../../Rendering/DebugUI.h"
#include "../../Rendering/CERenderer.h"
#include "../../Rendering/Resources/TextureFactory.h"

#include "../../Editor/Editor.h"

#include "../../Debug/CEProfiler.h"
#include "../../Debug/Console/Console.h"

#include "../../Memory/Memory.h"

#include "../../Core/Threading/ScopedLock.h"
#include "../../Core/Threading/ThreadSafeInt.h"
#include "../../Core/Threading/Platform/PlatformProcess.h"

bool EngineLoop::Init()
{
    TRACE_FUNCTION_SCOPE();

    GConsoleOutput = GenericOutputConsole::Create();
    if (!GConsoleOutput)
    {
        return false;
    }
    else
    {
        GConsoleOutput->SetTitle("ConceptEngine Error Console");
    }

    CEProfiler::Create();

    if (!Platform::Init())
    {
        CEPlatformMisc::MessageBox("ERROR", "Failed to init Platform");
        return false;
    }

    if (!CETaskManager::Get().Create())
    {
        return false;
    }

    if (!GEngine.Init())
    {
        return false;
    }

    // RenderAPI
    if (!RenderLayer::Init(ERenderLayerApi::D3D12))
    {
        return false;
    }

    if (!TextureFactory::Init())
    {
        return false;
    }

    // Init Application
    GApplication = CreateApplication();
    Assert(GApplication != nullptr);

    Platform::SetCallbacks(&GEngine);

    if (!GApplication->Init())
    {
        return false;
    }

    if (!CEInputManager::Get().Create())
    {
        return false;
    }

    if (!GRenderer.Create())
    {
        CEPlatformMisc::MessageBox("ERROR", "FAILED to create Renderer");
        return false;
    }

    GConsole.Create();

    if (!CEDebugUI::Create())
    {
        CEPlatformMisc::MessageBox("ERROR", "FAILED to create ImGuiContext");
        return false;
    }

    Editor::Init();

    return true;
}

void EngineLoop::Tick(CETimestamp Deltatime)
{
    TRACE_FUNCTION_SCOPE();

    Platform::Tick();

    GApplication->Tick(Deltatime);

    GConsole.Update();

    Editor::Tick();

    CEProfiler::Update();

    GRenderer.Update(*GApplication->Scene);
}

void EngineLoop::Run()
{
    Timer Timer;

    while (GEngine.IsRunning)
    {
        Timer.Tick();
        EngineLoop::Tick(Timer.GetDeltaTime());
    }
}

bool EngineLoop::Release()
{
    TRACE_FUNCTION_SCOPE();

    GCmdListExecutor.WaitForGPU();

    TextureFactory::Release();

    if (GApplication->Release())
    {
        SafeDelete(GApplication);
    }
    else
    {
        return false;
    }

    if (GEngine.Release())
    {
        Platform::SetCallbacks(nullptr);
    }
    else
    {
        return false;
    }

    CEDebugUI::Release();

    GRenderer.Release();

    RenderLayer::Release();

    CETaskManager::Get().Release();

    if (!Platform::Release())
    {
        return false;
    }

    SafeDelete(GConsoleOutput);

    return true;
}
