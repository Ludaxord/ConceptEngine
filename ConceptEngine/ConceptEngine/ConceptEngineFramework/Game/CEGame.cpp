#include "CEGame.h"

#include <spdlog/spdlog.h>


#include "CEConsole.h"
using namespace ConceptEngineFramework::Game;

static CEGame* g_pGame = nullptr;

/*
 * Instance for std::shared_ptr
 */
class CEConsoleInstance final : public CEConsole {

public:
	CEConsoleInstance(const std::wstring& windowName, int maxFileSizeInMB = 5, int maxFiles = 3,
	                  int maxConsoleLines = 500)
		: CEConsole(windowName, maxFileSizeInMB, maxFiles, maxConsoleLines) {
	}

	explicit CEConsoleInstance(const Logger& logger)
		: CEConsole(logger) {
	}
};

CEGame::CEGame(std::wstring name, HINSTANCE hInst) : m_hInstance(hInst), m_bIsRunning(false), m_requestQuit(false) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	CreateConsole(name);
	CreateInputDevices();
	CreateWindow(name);
}

CEGame& CEGame::Create(std::wstring name, HINSTANCE hInst) {
	if (!g_pGame) {
		g_pGame = new CEGame(name, hInst);
		spdlog::info("ConceptEngineFramework Game class created.");
	}

	return *g_pGame;
}

CEGame& CEGame::Get() {
	assert(g_pGame != nullptr);
	return *g_pGame;
}

uint32_t CEGame::Run() {
	assert(!m_bIsRunning);
	m_bIsRunning = true;

	MSG msg = {};
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		// m_inputManager.HandleMessage(msg);

		/*
		 * Check to see if application send quit signal
		 */
		if (m_requestQuit) {
			::PostQuitMessage(-2);
			m_requestQuit = false;
		}
	}

	m_bIsRunning = false;
	return static_cast<int32_t>(msg.wParam);
}


std::shared_ptr<CEConsole> CEGame::GetConsole() {
	return m_console;
}

void CEGame::CreateWindow(const std::wstring& windowName) {
	
}

std::shared_ptr<CEWindow> CEGame::CreateWindow(const std::wstring& windowName, int width, int height) {
	return nullptr;
}

void CEGame::CreateConsole(const std::wstring& windowName) {
	m_console = std::make_shared<CEConsoleInstance>(windowName);
	m_console->Create();
}

void CEGame::CreateInputDevices() {
	// m_keyboardDevice = m_inputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	// m_mouseDevice = m_inputManager.CreateDevice<gainput::InputDeviceMouse>();
	// for (unsigned i = 0; i < gainput::MaxPadCount; ++i) {
	// 	m_gamePadDevice[i] = m_inputManager.CreateDevice<gainput::InputDevicePad>(i);
	// }
	//
	// /*
	//  * prevent normalization of mouse coordinates
	//  */
	// m_inputManager.SetDisplaySize(1, 1);
}

LRESULT CEGame::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto res = WndProcHandler(hWnd, msg, wParam, lParam);
	return res ? *res : 0;
}


static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	/*
	 * Allow for external handling of window messages.
	 */
	if (CEGame::Get().OnWndProc(hwnd, message, wParam, lParam)) {
		return 1;
	}

	std::shared_ptr<CEScreen> pWindow;
	{
		// auto iter = gs_windowMap.find(hwnd);
		// if (iter != gs_windowMap.end()) {
		// 	pWindow = iter->second.lock();
		// }
	}

	if (pWindow) {
		switch (message) {
		case WM_DPICHANGED: {
			float dpiScaling = HIWORD(wParam) / 96.0f;
			DPIScaleEventArgs dpiScaleEventArgs(dpiScaling);
			// pWindow->OnDPIScaleChanged(dpiScaleEventArgs);
		}
						  break;
		case WM_PAINT: {
			/*
			 * Delta and total time will be filled in by Window.
			 */
			UpdateEventArgs updateEventArgs(0.0, 0.0);
			// pWindow->OnUpdate(updateEventArgs);
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
			if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR) {
				c = static_cast<unsigned int>(charMsg.wParam);
			}

			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			KeyCode key = (KeyCode)wParam;
			KeyEventArgs keyEventArgs(key, c, KeyState::Pressed, control, shift, alt);
			// pWindow->OnKeyPressed(keyEventArgs);
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

			KeyEventArgs keyEventArgs(key, c, KeyState::Released, control, shift, alt);
			// pWindow->OnKeyReleased(keyEventArgs);
		}
					 break;
					 /*
					 * Default window procedure will play a system notification sound
					 * when pressing Alt+Enter keyboard combination if this message is not handled
					 */
		case WM_SYSCHAR:
			break;
		case WM_KILLFOCUS: {
			/*
			 * window lost keyboard focus
			 */
			// EventArgs eventArgs;
			// pWindow->OnKeyboardBlur(eventArgs);
		}
						 break;
		case WM_SETFOCUS: {
			// EventArgs eventArgs;
			// pWindow->OnKeyboardFocus(eventArgs);
		}
						break;
		case WM_MOUSEMOVE: {
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));
			
			// MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
			// pWindow->OnMouseMoved(mouseMotionEventArgs);
		}
						 break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN: {
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			// Capture mouse movement until the button is released.
			SetCapture(hwnd);

			// MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Pressed, lButton,
			// 	mButton, rButton, control, shift, x, y);
			// pWindow->OnMouseButtonPressed(mouseButtonEventArgs);
		}
						   break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP: {
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			/*
			 * Stop capturing mouse
			 */
			ReleaseCapture();

			// MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Released, lButton,
			// 	mButton, rButton, control, shift, x, y);
			// pWindow->OnMouseButtonReleased(mouseButtonEventArgs);
		}
						 break;
		case WM_MOUSEWHEEL: {
			/*
			 * Distance mouse wheel is rotated.
			 * Positive value indicates wheel was rotated forwards (away user)
			 * Negative value indicates wheel was rotated backwards (toward user).
			 */
			float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
			short keyStates = (short)LOWORD(wParam);

			bool lButton = (keyStates & MK_LBUTTON) != 0;
			bool rButton = (keyStates & MK_RBUTTON) != 0;
			bool mButton = (keyStates & MK_MBUTTON) != 0;
			bool shift = (keyStates & MK_SHIFT) != 0;
			bool control = (keyStates & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			/*
			 * Convert screen coordinates to client coordinates.
			 */
			POINT screenToClientPoint;
			screenToClientPoint.x = x;
			screenToClientPoint.y = y;
			::ScreenToClient(hwnd, &screenToClientPoint);

			MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift,
				(int)screenToClientPoint.x, (int)screenToClientPoint.y);
			// pWindow->OnMouseWheel(mouseWheelEventArgs);
		}
						  break;
		case WM_CAPTURECHANGED: {
			EventArgs mouseBlurEventArgs;
			// pWindow->OnMouseBlur(mouseBlurEventArgs);
		}
							  break;
		case WM_MOUSEACTIVATE: {
			EventArgs mouseFocusEventArgs;
			// pWindow->OnMouseFocus(mouseFocusEventArgs);
		}
							 break;
		case WM_MOUSELEAVE: {
			EventArgs mouseLeaveEventArgs;
			// pWindow->OnMouseLeave(mouseLeaveEventArgs);
		}
						  break;
		case WM_SIZE: {
			// WindowState windowState = DecodeWindowState(wParam);

			int width = ((int)(short)LOWORD(lParam));
			int height = ((int)(short)HIWORD(lParam));

			// ResizeEventArgs resizeEventArgs(width, height, windowState);
			// pWindow->OnResize(resizeEventArgs);
		}
					break;
		case WM_CLOSE: {
			WindowCloseEventArgs windowCloseEventArgs;
			// pWindow->OnClose(windowCloseEventArgs);

			/*
			 * Check if user canceled close event
			 */
			if (windowCloseEventArgs.ConfirmClose) {
				pWindow->Hide();
			}
		}
					 break;
		case WM_DESTROY: {
			// std::lock_guard<std::mutex> lock(gs_windowHandlesMutex);
			// WindowMap::iterator iter = gs_windowMap.find(hwnd);
			// if (iter != gs_windowMap.end()) {
			// 	gs_windowMap.erase(iter);
			// }
		}
					   break;
		default:
			return ::DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else {
		switch (message) {
		case WM_CREATE:
			break;
		default:
			return ::DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}

	return 0;
}