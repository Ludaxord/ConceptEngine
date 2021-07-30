#pragma once
#include "../../CEDefinitions.h"

#include "../../Core/Input/InputCodes.h"
#include "../../Core/Application/Generic/CEGenericPlatform.h"
#include "../../Core/Application/Generic/GenericWindow.h"

struct KeyPressedEvent
{
    KeyPressedEvent(EKey InKey, bool InIsRepeat, const CEModifierKeyState& InModifiers)
        : Key(InKey)
        , IsRepeat(InIsRepeat)
        , Modifiers(InModifiers)
    {
    }

    EKey Key;
    bool IsRepeat;
    CEModifierKeyState Modifiers;
};

struct KeyReleasedEvent
{
    KeyReleasedEvent(EKey InKey, const CEModifierKeyState& InModifiers)
        : Key(InKey)
        , Modifiers(InModifiers)
    {
    }

    EKey             Key;
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
    MousePressedEvent(EMouseButton InButton, const CEModifierKeyState& InModifiers)
        : Button(InButton)
        , Modifiers(InModifiers)
    {
    }

    EMouseButton     Button;
    CEModifierKeyState Modifiers;
};

struct MouseReleasedEvent
{
    MouseReleasedEvent(EMouseButton InButton, const CEModifierKeyState& InModifiers)
        : Button(InButton)
        , Modifiers(InModifiers)
    {
    }

    EMouseButton     Button;
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
    WindowResizeEvent(const TRef<GenericWindow>& InWindow, uint16 InWidth, uint16 InHeight)
        : Window(InWindow)
        , Width(InWidth)
        , Height(InHeight)
    {
    }

    TRef<GenericWindow> Window;
    uint16 Width;
    uint16 Height;
};

struct WindowFocusChangedEvent
{
    WindowFocusChangedEvent(const TRef<GenericWindow>& InWindow, bool hasFocus)
        : Window(InWindow)
        , HasFocus(hasFocus)
    {
    }

    TRef<GenericWindow> Window;
    bool HasFocus;
};

struct WindowMovedEvent
{
    WindowMovedEvent(const TRef<GenericWindow>& InWindow, int16 x, int16 y)
        : Window(InWindow)
        , Position({ x, y })
    {
    }

    TRef<GenericWindow> Window;
    struct
    {
        int16 x;
        int16 y;
    } Position;
};

struct WindowMouseLeftEvent
{
    WindowMouseLeftEvent(const TRef<GenericWindow>& InWindow)
        : Window(InWindow)
    {
    }

    TRef<GenericWindow> Window;
};

struct WindowMouseEnteredEvent
{
    WindowMouseEnteredEvent(const TRef<GenericWindow>& InWindow)
        : Window(InWindow)
    {
    }

    TRef<GenericWindow> Window;
};

struct WindowClosedEvent
{
    WindowClosedEvent(const TRef<GenericWindow>& InWindow)
        : Window(InWindow)
    {
    }

    TRef<GenericWindow> Window;
};