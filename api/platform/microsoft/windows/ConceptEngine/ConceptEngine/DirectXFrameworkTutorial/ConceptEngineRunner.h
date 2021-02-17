#pragma once
#include <activation.h>
#include <dxgi.h>
#include <wrl.h>

#include "DirectXResources.h"
#include "DXSample.h"

namespace wrl = Microsoft::WRL;

class Tutorial;

class ConceptEngineRunner {
public:
	static int Run(std::shared_ptr<Tutorial> pTutorial, HINSTANCE hInstance, int nCmdShow);
	static int Run(DXSample* pTutorial, HINSTANCE hInstance, int nCmdShow);
	static HWND CreateMainWindow(std::shared_ptr<Tutorial> pTutorial, HINSTANCE hInstance);
	static HWND CreateSampleWindow(DXSample* pTutorial, HINSTANCE hInstance);
	static void ToggleFullScreenWindow(wrl::ComPtr<IDXGISwapChain> swapChain = nullptr);
	static void SetWindowZOrderToTopMost(bool setToTopMost);
	static bool IsFullScreen() { return m_fullScreen; }
	static HWND GetHWnd() { return m_hWnd; }
protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	inline static Logger m_logger = nullptr;
	inline static HWND m_hWnd = nullptr;
	static bool m_fullScreen;
	static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
	static RECT m_windowRect;
};
