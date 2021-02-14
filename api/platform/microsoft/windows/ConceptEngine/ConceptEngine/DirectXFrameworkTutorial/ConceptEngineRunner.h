#pragma once
#include <activation.h>
#include <dxgi.h>
#include <wrl.h>

namespace wrl = Microsoft::WRL;

class Tutorial;

class ConceptEngineRunner {
public:
	static int Run(Tutorial* pTutorial, HINSTANCE hInstance, int nCmdShow);
	static void ToggleFullScreenWindow(wrl::ComPtr<IDXGISwapChain> swapChain = nullptr);
	static void SetWindowZOrderToTopMost(bool setToTopMost);
	static bool IsFullScreen() { return m_fullScreen; }
	static HWND GetHWnd() { return m_hWnd; }
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	inline static HWND m_hWnd = nullptr;
	static bool m_fullScreen;
	static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
	static RECT m_windowRect;
};
