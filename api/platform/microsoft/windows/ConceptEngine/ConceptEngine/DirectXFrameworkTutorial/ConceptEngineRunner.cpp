#include "ConceptEngineRunner.h"

#include "Tutorial.h"

HWND ConceptEngineRunner::m_hWnd = nullptr;
bool ConceptEngineRunner::m_fullScreen = false;
RECT ConceptEngineRunner::m_windowRect;

int ConceptEngineRunner::Run(Tutorial* pTutorial, HINSTANCE hInstance, int nCmdShow) {
	try {
		// Parse the command line parameters

		// Initialize the window class.
		WNDCLASSEX windowClass = {0};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = pTutorial->GetTitle();
		RegisterClassEx(&windowClass);

		RECT windowRect = {0, 0, static_cast<LONG>(pTutorial->GetWidth()), static_cast<LONG>(pTutorial->GetHeight())};
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window and store a handle to it.
		m_hWnd = CreateWindow(
			windowClass.lpszClassName,
			pTutorial->GetTitle(),
			m_windowStyle,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, // We have no parent window.
			nullptr, // We aren't using menus.
			hInstance,
			pTutorial);

		// Initialize the sample. OnInit is defined in each child-implementation of DXSample.
		pTutorial->OnInit();

		ShowWindow(m_hWnd, nCmdShow);

		// Main sample loop.
		MSG msg = {};
		while (msg.message != WM_QUIT) {
			// Process any messages in the queue.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		pSample->OnDestroy();

		// Return this part of the WM_QUIT message to Windows.
		return static_cast<char>(msg.wParam);
	}
	catch (HrException& e) {
		if (e.Error() == E_APPLICATION_EXITING) {
			OutputDebugString(L"User initiated shutdown. Application is terminating.");
			pTutorial->OnDestroy();
			return 0;
		}

		OutputDebugString(L"Application hit a problem: ");
		OutputDebugStringA(e.what());
		OutputDebugString(L"\nTerminating.\n");

		pTutorial->OnDestroy();
		return EXIT_FAILURE;
	}
	catch (std::exception& e) {
		OutputDebugString(L"Application hit a problem: ");
		OutputDebugStringA(e.what());
		OutputDebugString(L"\nTerminating.\n");

		pTutorial->OnDestroy();
		return EXIT_FAILURE;
	}
}

// Convert a styled window into a fullscreen borderless window and back again.
void ConceptEngineRunner::ToggleFullScreenWindow(wrl::ComPtr<IDXGISwapChain> swapChain) {
	if (m_fullScreen) {
		// Restore the window's attributes and size.
		SetWindowLong(m_hWnd, GWL_STYLE, m_windowStyle);

		SetWindowPos(
			m_hWnd,
			HWND_NOTOPMOST,
			m_windowRect.left,
			m_windowRect.top,
			m_windowRect.right - m_windowRect.left,
			m_windowRect.bottom - m_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(m_hWnd, SW_NORMAL);
	}
	else {
		// Save the old window rect so we can restore it when exiting fullscreen mode.
		GetWindowRect(m_hWnd, &m_windowRect);

		// Make the window borderless so that the client area can fill the screen.
		SetWindowLong(m_hWnd, GWL_STYLE,
		              m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullscreenWindowRect;
		try {
			if (swapChain) {
				// Get the settings of the display on which the app's window is currently displayed
				wrl::ComPtr<IDXGIOutput> pOutput;
				ThrowIfFailed(swapChain->GetContainingOutput(&pOutput));
				DXGI_OUTPUT_DESC Desc;
				ThrowIfFailed(pOutput->GetDesc(&Desc));
				fullscreenWindowRect = Desc.DesktopCoordinates;
			}
			else {
				// Fallback to EnumDisplaySettings implementation
				throw HrException(S_FALSE);
			}
		}
		catch (HrException& e) {
			UNREFERENCED_PARAMETER(e);

			// Get the settings of the primary display
			DEVMODE devMode = {};
			devMode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

			fullscreenWindowRect = {
				devMode.dmPosition.x,
				devMode.dmPosition.y,
				devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
				devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
			};
		}

		SetWindowPos(
			m_hWnd,
			HWND_TOPMOST,
			fullscreenWindowRect.left,
			fullscreenWindowRect.top,
			fullscreenWindowRect.right,
			fullscreenWindowRect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);


		ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	m_fullScreen = !m_fullScreen;
}

void ConceptEngineRunner::SetWindowZOrderToTopMost(bool setToTopMost) {
	RECT windowRect;
	GetWindowRect(m_hWnd, &windowRect);

	SetWindowPos(
		m_hWnd,
		(setToTopMost) ? HWND_TOPMOST : HWND_NOTOPMOST,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		SWP_FRAMECHANGED | SWP_NOACTIVATE);
}

// Main message handler for the sample.
LRESULT CALLBACK ConceptEngineRunner::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
}
