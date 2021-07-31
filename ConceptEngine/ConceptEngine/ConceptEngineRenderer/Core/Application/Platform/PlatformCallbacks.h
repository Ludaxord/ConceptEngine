#pragma once
#include "../../../Core/Input/InputCodes.h"

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4100) // Disable unreferenced variable
#endif

class CEWindow;
struct CEModifierKeyState;

class PlatformCallbacks
{
public:
    virtual ~PlatformCallbacks() = default;

    virtual void OnKeyReleased(CEKey KeyCode, const CEModifierKeyState& ModierKeyState)
    {
    }

    virtual void OnKeyPressed(CEKey KeyCode, bool IsRepeat, const CEModifierKeyState& ModierKeyState)
    {
    }

    virtual void OnKeyTyped(uint32 Character)
    {
    }

    virtual void OnMouseMove(int32 x, int32 y)
    {
    }

    virtual void OnMouseReleased(CEMouseButton Button, const CEModifierKeyState& ModierKeyState)
    {
    }

    virtual void OnMousePressed(CEMouseButton Button, const CEModifierKeyState& ModierKeyState)
    {
    }

    virtual void OnMouseScrolled(float HorizontalDelta, float VerticalDelta)
    {
    }

    virtual void OnWindowResized(const CERef<CEWindow>& Window, uint16 Width, uint16 Height)
    {
    }

    virtual void OnWindowMoved(const CERef<CEWindow>& Window, int16 x, int16 y)
    {
    }
    
    virtual void OnWindowFocusChanged(const CERef<CEWindow>& Window, bool HasFocus)
    {
    }
    
    virtual void OnWindowMouseLeft(const CERef<CEWindow>& Window)
    {
    }
    
    virtual void OnWindowMouseEntered(const CERef<CEWindow>& Window)
    {
    }
    
    virtual void OnWindowClosed(const CERef<CEWindow>& Window)
    {
    }

    virtual void OnApplicationExit(int32 ExitCode)
    {
    }
};

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif