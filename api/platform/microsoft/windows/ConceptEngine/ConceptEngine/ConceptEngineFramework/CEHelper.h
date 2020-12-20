#pragma once

#define WIN32_LEAN_AND_MEAN
#include <exception>
#include <Windows.h> // For HRESULT

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

inline void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}