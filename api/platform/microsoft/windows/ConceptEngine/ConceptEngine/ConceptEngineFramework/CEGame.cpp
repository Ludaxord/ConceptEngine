#include "CEGame.h"

#include "CEDX12Libs.h"

#include "CEScreen.h"
#include "CETools.h"
#include "../ConceptEngineEditor/resource.h"

using namespace Concept::GameEngine;

static CEGame* g_pGameSingleton = nullptr;
constexpr wchar_t WINDOW_CLASS_NAME[] = L"RenderConceptEngineWindowClass";


constexpr int MAX_CONSOLE_LINES = 500;

using WindowMap = std::map<HWND, std::weak_ptr<CEScreen>>;
using WindowMapByName = std::map<std::wstring, std::weak_ptr<CEScreen>>;
static WindowMap gs_windowMap;
static WindowMapByName gs_windowMapByName;

static std::mutex gs_windowHandlesMutex;

/*
 * Wrapper struct to allow shared pointer for window class.
 * It is required because constructor and destructor for
 * Window class are protected and not accessible by std::make_shared method
 */
struct CEScreenInstance : public CEScreen {
	CEScreenInstance(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight) : CEScreen(
		hWnd, windowName, clientWidth, clientHeight) {
	}
};

/**
 * Create a console window (consoles are not automatically created for Windows
 * subsystems)
 */
static void CreateConsole() {
	// Allocate a console.
	if (AllocConsole()) {
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Increase screen buffer to allow more lines of text than the default.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
		consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
		SetConsoleCursorPosition(lStdHandle, {0, 0});

		// Redirect unbuffered STDOUT to the console.
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		FILE* fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stdout);
		setvbuf(stdout, nullptr, _IONBF, 0);

		// Redirect unbuffered STDIN to the console.
		lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "r");
		freopen_s(&fp, "CONIN$", "r", stdin);
		setvbuf(stdin, nullptr, _IONBF, 0);

		// Redirect unbuffered STDERR to the console.
		lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		freopen_s(&fp, "CONOUT$", "w", stderr);
		setvbuf(stderr, nullptr, _IONBF, 0);

		// Clear the error state for each of the C++ standard stream objects. We
		// need to do this, as attempts to access the standard streams before
		// they refer to a valid target will cause the iostream objects to enter
		// an error state. In versions of Visual Studio after 2005, this seems
		// to always occur during startup regardless of whether anything has
		// been read from or written to the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}
}

CEGame& CEGame::Create(HINSTANCE hInst) {
	if (!g_pGameSingleton) {
		g_pGameSingleton = new CEGame(hInst);
		spdlog::info("Concept Engine Game class created.");
	}

	return *g_pGameSingleton;
}

void CEGame::Destroy() {
	if (g_pGameSingleton) {
		delete g_pGameSingleton;
		g_pGameSingleton = nullptr;
		spdlog::info("Concept Engine Game class destroyed.");
	}
}

CEGame& CEGame::Get() {
	assert(g_pGameSingleton != nullptr);
	return *g_pGameSingleton;
}

/**
 * Create loggers
 * @see https://github.com/gabime/spdlog#asynchronous-logger-with-multi-sinks
 */
Logger CEGame::CreateLogger(const std::string& name) {
	Logger logger = spdlog::get(name);
	if (!logger) {
		logger = m_logger->clone(name);
		spdlog::register_logger(logger);
	}
	return logger;
}

gainput::DeviceId CEGame::GetKeyboardId() const {
	return m_keyboardDevice;
}

gainput::DeviceId CEGame::GetMouseId() const {
	return m_mouseDevice;
}

gainput::DeviceId CEGame::GetPadId(unsigned index) const {
	assert(index >= 0 && index < gainput::MaxPadCount);
	return m_gamepadDevice[index];
}

std::shared_ptr<gainput::InputMap> CEGame::CreateInputMap(const char* name) {
	return std::make_shared<gainput::InputMap>(m_inputManager, name);
}

int32_t CEGame::Run() {
	assert(!m_bIsRunning);
	m_bIsRunning = true;

	MSG msg = {};
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		m_inputManager.HandleMessage(msg);

		/*
		 * Check to see if application send quit signal
		 */
		if (m_requestQuit) {
			::PostQuitMessage(0);
			m_requestQuit = false;
		}
	}

	m_bIsRunning = false;
	return static_cast<int32_t>(msg.wParam);
}

void CEGame::SetDisplaySize(int width, int height) {
	m_inputManager.SetDisplaySize(width, height);
}

void CEGame::ProcessInput() {
	m_inputManager.Update();
}

void CEGame::Stop() {
	/*
	 * When called from another threwad other than main thread,
	 * WM_QUIT message goes to that thread and will not be handled in main thread.
	 * To circumvent this, it is set boolean flag to indicate that user has requested to quit application.
	 */
	m_requestQuit = true;
}

void CEGame::RegisterDirectoryChangeListener(const std::wstring& dir, bool recursive) {
	std::scoped_lock lock(m_directoryChangeMutex);
	m_directoryChanges.AddDirectory(dir, recursive, FILE_NOTIFY_CHANGE_LAST_WRITE);
}

std::shared_ptr<CEScreen> CEGame::CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight) {
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = {0, 0, static_cast<LONG>(clientWidth), static_cast<LONG>(clientHeight)};

	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	uint32_t width = windowRect.right - windowRect.left;
	uint32_t height = windowRect.bottom - windowRect.top;

	int windowX = std::max<int>(0, (screenWidth - (int)width) / 2);
	int windowY = std::max<int>(0, (screenHeight - (int)height) / 2);

	HWND hWindow = ::CreateWindowExW(NULL, WINDOW_CLASS_NAME, windowName.c_str(), WS_OVERLAPPEDWINDOW, windowX, windowY,
	                                 width, height, NULL, NULL, m_hInstance, NULL);

	if (!hWindow) {
		spdlog::error("Failed to create window");
		return nullptr;
	}

	auto pWindow = std::make_shared<CEScreenInstance>(hWindow, windowName, clientWidth, clientHeight);

	gs_windowMap.insert(WindowMap::value_type(hWindow, pWindow));
	gs_windowMapByName.insert(WindowMapByName::value_type(windowName, pWindow));

	return pWindow;
}

std::shared_ptr<CEScreen> CEGame::GetWindowByName(const std::wstring& windowName) const {
	auto iter = gs_windowMapByName.find(windowName);
	return (iter != gs_windowMapByName.end()) ? iter->second.lock() : nullptr;
}

CEGame::CEGame(HINSTANCE hInst) : m_hInstance(hInst), m_bIsRunning(false), m_requestQuit(false),
                                  m_bTerminateDirectoryChangeThread(false) {
	// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
	// Using this awareness context allows the client area of the window
	// to achieve 100% scaling while still allowing non-client window content to
	// be rendered in a DPI sensitive fashion.
	// @see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setthreaddpiawarenesscontext
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	CreateConsoleWindow();
	CreateInputDevices();
	RegisterWindowClass();

	/*
	 * Create thread to listen fo file changes
	 */
	m_directoryChangerListenerThread = std::thread(&CEGame::CheckFileChanges, this);
	SetThreadName(m_directoryChangerListenerThread, "Check file Change");
}

CEGame::~CEGame() {
	/*
	 * Close thread listening for file changes
	 */
	m_bTerminateDirectoryChangeThread = true;
	if (m_directoryChangerListenerThread.joinable()) {
		m_directoryChangerListenerThread.join();
	}

	gs_windowMap.clear();
	gs_windowMapByName.clear();
}

void CEGame::OnFileChange(FileChangedEventArgs& e) {
	FileChanged(e);
}

LRESULT CEGame::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto res = WndProcHandler(hWnd, msg, wParam, lParam);
	return res ? *res : 0;
}

void CEGame::OnExit(EventArgs& e) {
	/*
	 * Exit event
	 */
	Exit(e);
}

/*
 * Directory change listener thread entry point.
 */
void CEGame::CheckFileChanges() {
	while (!m_bTerminateDirectoryChangeThread) {
		std::scoped_lock lock(m_directoryChangeMutex);

		DWORD waitSignal = ::WaitForSingleObject(m_directoryChanges.GetWaitHandle(), 0);
		switch (waitSignal) {
		case WAIT_OBJECT_0:
			/*
			 * File has been modified
			 */
			if (m_directoryChanges.CheckOverflow()) {
				/*
				 * This could happen if a lot of modifications occur at once
				 */
				spdlog::warn("Directory change overflow occurred");
			}
			else {
				DWORD action;
				std::wstring fileName;
				m_directoryChanges.Pop(action, fileName);
				FileAction fileAction = FileAction::Unknown;
				switch (action) {
				case FILE_ACTION_ADDED:
					fileAction = FileAction::Added;
					break;
				case FILE_ACTION_REMOVED:
					fileAction = FileAction::Removed;
					break;
				case FILE_ACTION_MODIFIED:
					fileAction = FileAction::Modified;
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					fileAction = FileAction::RenameOld;
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					fileAction = FileAction::RenameNew;
					break;
				default:
					break;
				}
				FileChangedEventArgs fileChangedEventArgs(fileAction, fileName);
				OnFileChange(fileChangedEventArgs);
			}
			break;
		default:
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void CEGame::CreateConsoleWindow() {
#if defined(_DEBUG)
	/*
	 * Create console window for std::cout
	 */
	CreateConsole();
#endif

	/*
	 * Init spdlog
	 */
	spdlog::init_thread_pool(8192, 1);
	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"logs/log.txt", 1024 * 1024 * 5, 3, true); // Max size: 5MB, Max files: 3, Rotate on open: true
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink, msvc_sink};
	m_logger = std::make_shared<spdlog::async_logger>("ConceptEngine", sinks.begin(), sinks.end(),
	                                                  spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	spdlog::register_logger(m_logger);
	spdlog::set_default_logger(m_logger);
	spdlog::flush_on(spdlog::level::info);
}

void CEGame::CreateInputDevices() {
	m_keyboardDevice = m_inputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	m_mouseDevice = m_inputManager.CreateDevice<gainput::InputDeviceMouse>();
	for (unsigned i = 0; i < gainput::MaxPadCount; ++i) {
		m_gamepadDevice[i] = m_inputManager.CreateDevice<gainput::InputDevicePad>(i);
	}

	/*
	 * prevent normalization of mouse coordinates
	 */
	m_inputManager.SetDisplaySize(1, 1);
}

void CEGame::RegisterWindowClass() {
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		_com_error err(hr);
		std::wstring werrorMessage(err.ErrorMessage());
		std::string errorMessage(werrorMessage.begin(), werrorMessage.end());
		spdlog::critical("CoInitialize failed: {}", errorMessage);
		throw new std::exception(errorMessage.c_str());
	}

	WNDCLASSEXW wndClass = {0};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = m_hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = WINDOW_CLASS_NAME;
	wndClass.hIconSm = static_cast<HICON>(LoadImage(m_hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

	if (!RegisterClassExW(&wndClass)) {
		MessageBoxA(NULL, "Unable to register window class.", "Error", MB_OK | MB_ICONERROR);
	}
}

/*
 * Convert mouse ID int MouseButton ID
 */
static MouseButton DecodeMouseButton(UINT messageID) {
	MouseButton mouseButton = MouseButton::None;
	switch (messageID) {
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK: {
		mouseButton = MouseButton::Left;
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK: {
		mouseButton = MouseButton::Right;
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK: {
		mouseButton = MouseButton::Middle;
	}
	break;
	}
	return mouseButton;
}

/*
 * Convert message ID into ButtonState.
 */
static ButtonState DecodeButtonState(UINT messageID) {
	ButtonState buttonState = ButtonState::Pressed;
	switch (messageID) {
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		buttonState = ButtonState::Released;
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		buttonState = ButtonState::Pressed;
		break;
	}

	return buttonState;
}

/*
 * Convert wParam of WM_SIZE events to WindowState.
 */
static WindowState DecodeWindowState(WPARAM wParam) {
	WindowState windowState = WindowState::Restored;

	switch (wParam) {
	case SIZE_RESTORED:
		windowState = WindowState::Restored;
		break;
	case SIZE_MINIMIZED:
		windowState = WindowState::Minimized;
		break;
	case SIZE_MAXIMIZED:
		windowState = WindowState::Maximized;
		break;
	default:
		break;
	}

	return windowState;
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
		auto iter = gs_windowMap.find(hwnd);
		if (iter != gs_windowMap.end()) {
			pWindow = iter->second.lock();
		}
	}

	if (pWindow) {
		switch (message) {
		case WM_DPICHANGED: {
			float dpiScaling = HIWORD(wParam) / 96.0f;
			DPIScaleEventArgs dpiScaleEventArgs(dpiScaling);
			pWindow->OnDPIScaleChanged(dpiScaleEventArgs);
		}
		break;
		case WM_PAINT: {
			/*
			 * Delta and total time will be filled in by Window.
			 */
			UpdateEventArgs updateEventArgs(0.0, 0.0);
			pWindow->OnUpdate(updateEventArgs);
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
			pWindow->OnKeyPressed(keyEventArgs);
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
			pWindow->OnKeyReleased(keyEventArgs);
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
			EventArgs eventArgs;
			pWindow->OnKeyboardBlur(eventArgs);
		}
		break;
		case WM_SETFOCUS: {
			EventArgs eventArgs;
			pWindow->OnKeyboardFocus(eventArgs);
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

			MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
			pWindow->OnMouseMoved(mouseMotionEventArgs);
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

			/*
			 * Capture mouse movement until button is released.
			 */
			SetCapture(hwnd);

			MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Pressed, lButton,
			                                          mButton, rButton, control, shift, x, y);
			pWindow->OnMouseButtonPressed(mouseButtonEventArgs);
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

			MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), ButtonState::Released, lButton,
			                                          mButton, rButton, control, shift, x, y);
			pWindow->OnMouseButtonReleased(mouseButtonEventArgs);
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
			pWindow->OnMouseWheel(mouseWheelEventArgs);
		}
		break;
		case WM_CAPTURECHANGED: {
			EventArgs mouseBlurEventArgs;
			pWindow->OnMouseBlur(mouseBlurEventArgs);
		}
		break;
		case WM_MOUSEACTIVATE: {
			EventArgs mouseFocusEventArgs;
			pWindow->OnMouseFocus(mouseFocusEventArgs);
		}
		break;
		case WM_MOUSELEAVE: {
			EventArgs mouseLeaveEventArgs;
			pWindow->OnMouseLeave(mouseLeaveEventArgs);
		}
		break;
		case WM_SIZE: {
			WindowState windowState = DecodeWindowState(wParam);

			int width = ((int)(short)LOWORD(lParam));
			int height = ((int)(short)HIWORD(lParam));

			ResizeEventArgs resizeEventArgs(width, height, windowState);
			pWindow->OnResize(resizeEventArgs);
		}
		break;
		case WM_CLOSE: {
			WindowCloseEventArgs windowCloseEventArgs;
			pWindow->OnClose(windowCloseEventArgs);

			/*
			 * Check if user canceled close event
			 */
			if (windowCloseEventArgs.ConfirmClose) {
				pWindow->Hide();
			}
		}
		break;
		case WM_DESTROY: {
			std::lock_guard<std::mutex> lock(gs_windowHandlesMutex);
			WindowMap::iterator iter = gs_windowMap.find(hwnd);
			if (iter != gs_windowMap.end()) {
				gs_windowMap.erase(iter);
			}
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
