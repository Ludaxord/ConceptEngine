#pragma once
#include "../../../Core/Input/InputCodes.h"
#include "../../../Core/Application/Generic/CEGenericPlatform.h"
#include "../../../Core/Application/Platform/PlatformCallbacks.h"
#include "../../../Core/Containers/Array.h"

#include "WindowsWindow.h"

class WindowsCursor;

struct WindowsEvent
{
    WindowsEvent(HWND InWindow, uint32 InMessage, WPARAM InwParam, LPARAM InlParam)
        : Window(InWindow)
        , Message(InMessage)
        , wParam(InwParam)
        , lParam(InlParam)
    {
    }

    HWND   Window;
    uint32 Message;
    WPARAM wParam; 
    LPARAM lParam;
};

struct WindowHandle
{
    WindowHandle() = default;

    WindowHandle(HWND InHandle)
        : Handle(InHandle)
    {
    }

    WindowsWindow* GetWindow() const
    {
        WindowsWindow* Window = (WindowsWindow*)GetWindowLongPtrA(Handle, GWLP_USERDATA);
        if (Window)
        {
            Window->AddRef();
        }

        return Window;
    }

    HWND Handle;
};

class WindowsPlatform : public CEGenericPlatform
{
public:
    static void PreMainInit(HINSTANCE InInstance);

    static bool Init();

    static void Tick();

    static bool Release();

    static CEModifierKeyState GetModifierKeyState();

    static void SetCapture(CEWindow* Window);
    static void SetActiveWindow(CEWindow* Window);

    static CEWindow* GetCapture();
    static CEWindow* GetActiveWindow();

    static void SetCursor(GenericCursor* Cursor);
    static GenericCursor* GetCursor();

    static void SetCursorPos(CEWindow* RelativeWindow, int32 x, int32 y);
    static void GetCursorPos(CEWindow* RelativeWindow, int32& OutX, int32& OutY);

    static LPCWSTR GetWindowClassName() { return L"WindowClass"; }
    static HINSTANCE GetInstance()     { return Instance; }
    
    static void StoreMessage(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam);

private:
    static bool InitCursors();

    static bool RegisterWindowClass();
    
    static LRESULT MessageProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam);
    static void HandleStoredMessage(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam);

private:
    static CEArray<WindowsEvent> Messages;
    
    static CERef<WindowsCursor> CurrentCursor;

    static bool IsTrackingMouse;

    static HINSTANCE Instance;
};