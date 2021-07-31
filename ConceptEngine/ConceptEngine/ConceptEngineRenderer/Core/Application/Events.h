#pragma once
#include "../../CEDefinitions.h"

#include "../../Core/Input/InputCodes.h"
#include "../../Core/Application/Generic/CEGenericPlatform.h"
#include "../../Core/Application/Generic/CEWindow.h"

struct KeyPressedEvent
{
    KeyPressedEvent(CEKey InKey, bool InIsRepeat, const CEModifierKeyState& InModifiers)
        : Key(InKey)
        , IsRepeat(InIsRepeat)
        , Modifiers(InModifiers)
    {
    }

    CEKey Key;
    bool IsRepeat;
    CEModifierKeyState Modifiers;
};

struct KeyReleasedEvent
{
    KeyReleasedEvent(CEKey InKey, const CEModifierKeyState& InModifiers)
        : Key(InKey)
        , Modifiers(InModifiers)
    {
    }

    CEKey             Key;
    CEModifierKeyState Modifiers;
};

struct KeyTypedEvent
{
    KeyTypedEvent(uint32 InCharacter)
        : Character(InCharacter)
    {
    }

    FORCEINLINE const char GetPrintableCharacter() const
    {
        return static_cast<char>(Character);
    }

    uint32 Character;
};

struct MouseMovedEvent
{
    MouseMovedEvent(int32 InX, int32 InY)
        : x(InX)
        , y(InY)
    {
    }

    int32 x;
    int32 y;
};

struct MousePressedEvent
{
    MousePressedEvent(CEMouseButton InButton, const CEModifierKeyState& InModifiers)
        : Button(InButton)
        , Modifiers(InModifiers)
    {
    }

    CEMouseButton     Button;
    CEModifierKeyState Modifiers;
};

struct MouseReleasedEvent
{
    MouseReleasedEvent(CEMouseButton InButton, const CEModifierKeyState& InModifiers)
        : Button(InButton)
        , Modifiers(InModifiers)
    {
    }

    CEMouseButton     Button;
    CEModifierKeyState Modifiers;
};

struct MouseScrolledEvent
{
    MouseScrolledEvent(float InHorizontalDelta, float InVerticalDelta)
        : HorizontalDelta(InHorizontalDelta)
        , VerticalDelta(InVerticalDelta)
    {
    }

    float HorizontalDelta;
    float VerticalDelta;
};

struct WindowResizeEvent
{
    WindowResizeEvent(const CERef<CEWindow>& InWindow, uint16 InWidth, uint16 InHeight)
        : Window(InWindow)
        , Width(InWidth)
        , Height(InHeight)
    {
    }

    CERef<CEWindow> Window;
    uint16 Width;
    uint16 Height;
};

struct WindowFocusChangedEvent
{
    WindowFocusChangedEvent(const CERef<CEWindow>& InWindow, bool hasFocus)
        : Window(InWindow)
        , HasFocus(hasFocus)
    {
    }

    CERef<CEWindow> Window;
    bool HasFocus;
};

struct WindowMovedEvent
{
    WindowMovedEvent(const CERef<CEWindow>& InWindow, int16 x, int16 y)
        : Window(InWindow)
        , Position({ x, y })
    {
    }

    CERef<CEWindow> Window;
    struct
    {
        int16 x;
        int16 y;
    } Position;
};

struct WindowMouseLeftEvent
{
    WindowMouseLeftEvent(const CERef<CEWindow>& InWindow)
        : Window(InWindow)
    {
    }

    CERef<CEWindow> Window;
};

struct WindowMouseEnteredEvent
{
    WindowMouseEnteredEvent(const CERef<CEWindow>& InWindow)
        : Window(InWindow)
    {
    }

    CERef<CEWindow> Window;
};

struct WindowClosedEvent
{
    WindowClosedEvent(const CERef<CEWindow>& InWindow)
        : Window(InWindow)
    {
    }

    CERef<CEWindow> Window;
};