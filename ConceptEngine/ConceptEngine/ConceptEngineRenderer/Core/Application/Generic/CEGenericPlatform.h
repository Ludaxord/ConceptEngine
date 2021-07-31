#pragma once
#include "../../../Core/CERef.h"
#include "../../../Core/Input/InputCodes.h"

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4100) // Disable unreferenced variable
#endif

class CEPlatformCallbacks;
class CEWindow;
class CECursor;

struct CEModifierKeyState
{
public:
    CEModifierKeyState() = default;

    CEModifierKeyState(uint32 InModifierMask)
        : ModifierMask(InModifierMask)
    {
    }

    bool IsCtrlDown() const { return (ModifierMask & ModifierFlag_Ctrl); }
    bool IsAltDown() const { return (ModifierMask & ModifierFlag_Alt); }
    bool IsShiftDown() const { return (ModifierMask & ModifierFlag_Shift); }
    bool IsCapsLockDown() const { return (ModifierMask & ModifierFlag_CapsLock); }
    bool IsSuperKeyDown() const { return (ModifierMask & ModifierFlag_Super); }
    bool IsNumPadDown() const { return (ModifierMask & ModifierFlag_NumLock); }

    uint32 ModifierMask = 0;
};

class CEGenericPlatform
{
public:
    static bool Init() { return false; }

    static void Tick() {}

    static bool Release() { return false; }

    static CEModifierKeyState GetModifierKeyState() { return CEModifierKeyState(); }

    static void SetCapture(CEWindow* Window) {}
    static void SetActiveWindow(CEWindow* Window) {}

    static CEWindow* GetCapture() { return nullptr; }
    static CEWindow* GetActiveWindow() { return nullptr; }

    static void SetCursor(CECursor* Cursor) {}
    static CECursor* GetCursor() { return nullptr; }

    static void SetCursorPos(CEWindow* RelativeWindow, int32 x, int32 y) {}
    static void GetCursorPos(CEWindow* RelativeWindow, int32& OutX, int32& OutY) {}

    static void SetCallbacks(CEPlatformCallbacks* InCallbacks);
    static CEPlatformCallbacks* GetCallbacks();

protected:
    static CEPlatformCallbacks* Callbacks;
};

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif