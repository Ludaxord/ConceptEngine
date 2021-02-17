#include "ConceptEngineRunner.h"

#include <sstream>

#include "DirectXHelper.h"
#include "Tutorial.h"
#include "../ConceptEngineTutorialRunner/resource.h"

bool ConceptEngineRunner::m_fullScreen = false;
RECT ConceptEngineRunner::m_windowRect;

int ConceptEngineRunner::Run(std::shared_ptr<Tutorial> pTutorial, HINSTANCE hInstance, int nCmdShow) {
	try {
		m_logger = pTutorial->CreateLogger("Box Tutorial");
		m_hWnd = CreateMainWindow(pTutorial, hInstance);
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

		pTutorial->OnDestroy();

		// Return this part of the WM_QUIT message to Windows.
		return static_cast<char>(msg.wParam);
	}
	catch (HrException& e) {
		if (e.Error() == E_APPLICATION_EXITING) {
			spdlog::error("User initiated shutdown. Application is terminating.");
			pTutorial->OnDestroy();
			return 0;
		}

		spdlog::error("Application hit a problem: ");
		spdlog::error(e.what());
		spdlog::error("Terminating.");

		pTutorial->OnDestroy();

		std::ostringstream oss;
		oss << "Application hit a problem: " << e.what() << std::endl;
		MessageBoxA(m_hWnd, oss.str().c_str(), "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	catch (std::exception& e) {
		spdlog::error("Application hit a problem: ");
		spdlog::error(e.what());
		spdlog::error("Terminating.");

		pTutorial->OnDestroy();

		std::ostringstream oss;
		oss << "Application hit a problem: " << e.what() << std::endl;
		MessageBoxA(m_hWnd, oss.str().c_str(), "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
}

int ConceptEngineRunner::Run(DXSample* pTutorial, HINSTANCE hInstance, int nCmdShow) {
	try {
		m_hWnd = CreateSampleWindow(pTutorial, hInstance);
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

		pTutorial->OnDestroy();

		// Return this part of the WM_QUIT message to Windows.
		return static_cast<char>(msg.wParam);
	}
	catch (HrException& e) {
		if (e.Error() == E_APPLICATION_EXITING) {
			spdlog::error("User initiated shutdown. Application is terminating.");
			pTutorial->OnDestroy();
			return 0;
		}

		spdlog::error("Application hit a problem: ");
		spdlog::error(e.what());
		spdlog::error("Terminating.");

		pTutorial->OnDestroy();

		std::ostringstream oss;
		oss << "Application hit a problem: " << e.what() << std::endl;
		MessageBoxA(m_hWnd, oss.str().c_str(), "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	catch (std::exception& e) {
		spdlog::error("Application hit a problem: ");
		spdlog::error(e.what());
		spdlog::error("Terminating.");

		pTutorial->OnDestroy();

		std::ostringstream oss;
		oss << "Application hit a problem: " << e.what() << std::endl;
		MessageBoxA(m_hWnd, oss.str().c_str(), "Error", MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
}

HWND ConceptEngineRunner::CreateMainWindow(std::shared_ptr<Tutorial> pTutorial, HINSTANCE hInstance) {

	// Initialize the window class.
	WNDCLASSEX windowClass = {0};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = pTutorial->GetTitle().c_str();
	//TODO: Load icons to project
	windowClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&windowClass);

	RECT windowRect = {0, 0, static_cast<LONG>(pTutorial->GetWidth()), static_cast<LONG>(pTutorial->GetHeight())};
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	HWND hwnd = CreateWindow(
		windowClass.lpszClassName,
		pTutorial->GetTitle().c_str(),
		m_windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, // We have no parent window.
		nullptr, // We aren't using menus.
		hInstance,
		pTutorial.get());

	return hwnd;
}

HWND ConceptEngineRunner::CreateSampleWindow(DXSample* pTutorial, HINSTANCE hInstance) {

	// Initialize the window class.
	WNDCLASSEX windowClass = {0};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = pTutorial->GetTitle();
	//TODO: Load icons to project
	windowClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&windowClass);

	RECT windowRect = {0, 0, static_cast<LONG>(pTutorial->GetWidth()), static_cast<LONG>(pTutorial->GetHeight())};
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	HWND hwnd = CreateWindow(
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

	return hwnd;
}

// Convert a styled window into a fullscreen borderless window and back again.
//TODO: Move to tutorial class
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
	Tutorial* pTutorial = reinterpret_cast<Tutorial*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message) {
	case WM_CREATE: {
		//Save Tutorial class passed in to CreateWindow
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN: {
		MSG charMsg;

		/*
		 * Get Unicode char (UTF-16)
		 */
		unsigned int c = 0;

		/*
		 * For printable characters, next message will be WM_CHAR;
		 * This message contains character code we need to send KeyPresent event.
		 * Inspired by SDL 1.2 implementation
		 */
		if (PeekMessage(&charMsg, hWnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR) {
			c = static_cast<unsigned int>(charMsg.wParam);
		}

		bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		KeyCode key = (KeyCode)wParam;
		pTutorial->OnKeyDown(key);
	}
	break;
	case WM_SYSKEYUP:
	case WM_KEYUP: {
		bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

		KeyCode key = (KeyCode)wParam;
		unsigned int c = 0;
		unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

		/*
		 * Determine which key was released byt converting key code and scan code to printable character (if possible).
		 * Inspired by SDL 1.2 implementation
		 */
		unsigned char keyboardState[256];
		GetKeyboardState(keyboardState);
		wchar_t translatedCharacters[4];
		if (int result = ToUnicodeEx((UINT)wParam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0) {
			c = translatedCharacters[0];
		}

		//Toggle full screen with alt+enter
		//TODO: move to tutorial class
		if ((wParam == VK_RETURN) && (lParam & (1 << 29))) {
			if (pTutorial && pTutorial->GetDirectXResources()->IsTearingSupported()) {
				ToggleFullScreenWindow(pTutorial->GetSwapChain());
				return 0;
			}
		}

		pTutorial->OnKeyUp(key);
	}
	case WM_PAINT:
		if (pTutorial) {
			pTutorial->OnUpdate();
			pTutorial->OnRender();
		}
		break;
	case WM_SIZE:
		if (pTutorial) {
			RECT windowRect = {};
			GetWindowRect(hWnd, &windowRect);
		}
		break;
	case WM_MOVE:
		break;
	case WM_DISPLAYCHANGE:
		if (pTutorial) {
			pTutorial->OnDisplayChanged();
		}
		break;
	case WM_MOUSEMOVE:
		if (pTutorial && static_cast<UINT8>(wParam) == MK_LBUTTON) {
			UINT x = LOWORD(lParam);
			UINT y = HIWORD(lParam);
			pTutorial->OnMouseMove(x, y);
		}
		break;
	case WM_LBUTTONDOWN: {
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pTutorial->OnMouseButtonDown(KeyCode::LButton, x, y);
	}
	break;
	case WM_LBUTTONUP: {
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pTutorial->OnMouseButtonUp(KeyCode::LButton, x, y);
	}
	break;
	case WM_RBUTTONDOWN: {
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pTutorial->OnMouseButtonDown(KeyCode::RButton, x, y);
	}
	break;
	case WM_RBUTTONUP: {
		UINT x = LOWORD(lParam);
		UINT y = HIWORD(lParam);
		pTutorial->OnMouseButtonUp(KeyCode::RButton, x, y);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
