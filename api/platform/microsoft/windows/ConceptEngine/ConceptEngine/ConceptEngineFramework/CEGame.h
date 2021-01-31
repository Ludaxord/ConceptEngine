#pragma once
#include <memory>
#include <gainput/gainput.h>
#include <spdlog/logger.h>


#include "CEEvents.h"
#include "ReadDirectoryChanges.h"


#ifdef CreateWindow
#undef CreateWindow
#endif

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


namespace Concept::GameEngine {

	class CEScreen;
	using Logger = std::shared_ptr<spdlog::logger>;

	/*
	 * Windows message handler
	 */
	using CEWindowProcEvent = Delegate<LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)>;

	class CEGame {
	public:
		/*
		 * Create singleton application instance
		 * @param hInst, application Instance
		 * @returns reference to created instance
		 */
		static CEGame& Create(HINSTANCE hInst);

		/*
		 * Destroy Application instance.
		 */
		static void Destroy();

		/*
		 * Get reference to application instance
		 * @returns reference to application instance.
		 */
		static CEGame& Get();

		/*
		 * Create named logger or get previously created logger with same name 
		 */
		Logger CreateLogger(const std::string& name);

		/*
		 * Get keyboard device ID.
		 */
		gainput::DeviceId GetKeyboardId() const;

		/*
		 * Get mouse device ID.
		 */
		gainput::DeviceId GetMouseId() const;

		/*
		 * Get gamepad device ID.
		 *
		 * @param index, index of connected pad [0 ... gainput::MaxPadCount]
		 */
		gainput::DeviceId GetPadId(unsigned index = 0) const;

		/*
		 * Get input device
		 *
		 * @param [name] (Optional) name of input map.
		 * @see http://gainput.johanneskuhlmann.de/api/classgainput_1_1InputMap.html
		 */
		std::shared_ptr<gainput::InputMap> CreateInputMap(const char* name = nullptr);

		/*
		 * Start main application run loop.
		 *
		 * @returns error code (if error occurred)
		 */
		int32_t Run();

		/*
		 * Inform input manager of changes to size of display,
		 * This is needed for gainput ot normalize mouse inputs.
		 * NOTE: Only use this on single window Resize event!
		 *
		 * @param width, width of window client area (in pixels)
		 * @param height, height of window client area (in pixels)
		 */
		void SetDisplaySize(int width, int height);

		/*
		 * Process pad and keyboard events, This should be called once per frame before updating game logic.
		 */
		void ProcessInput();

		/*
		 * Stop application
		 */
		void Stop();

		/*
		 * To support hot loading of modified files, you can register directory path for listening for file change notifications, File change notifications are set through ::FileChange event.
		 * @param dir, directory to listen for file changes.
		 * @param recursive whether to listen for file changes in sub-folders
		 */
		void RegisterDirectoryChangeListener(const std::wstring& dir, bool recursive = true);

		/*
		 * Create a render window.
		 *
		 * @param windowName, name of window instance. This will also be name that appears in title of window.
		 * @param clientWidth, width (in pixels) of window client area.
		 * @param clientHeight, height (in pixels) of window client area.
		 * @returns created window instance.
		 */
		std::shared_ptr<CEScreen> CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight);

		/*
		 * Get window by name
		 *
		 * @param windowName, name that was used to create window.
		 */
		std::shared_ptr<CEScreen> GetWindowByName(const std::wstring& windowName) const;

		/*
		 * Invoked when message is sent to window.
		 */
		CEWindowProcEvent WndProcHandler;

		/*
		 * Invoked when file is modified on disk
		 */
		FileChangeEvent FileChanged;

		/*
		 * Application is exiting
		 */
		Event Exit;

	protected:
		friend LRESULT CALLBACK ::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		CEGame(HINSTANCE hInst);
		virtual ~CEGame();

		/*
		 * File modification was detected
		 */
		virtual void OnFileChange(FileChangedEventArgs& e);

		/*
		 * Windows message handler
		 */
		virtual LRESULT OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		/*
		 * Application is going to close
		 */
		virtual void OnExit(EventArgs& e);
	private:
		/*
		 * Private and deleted.
		 */
		CEGame(const CEGame&) = delete;
		CEGame(CEGame&&) = delete;
		CEGame& operator=(CEGame&) = delete;
		CEGame& operator=(CEGame&&) = delete;

		/*
		 * Create console window
		 */
		void CreateConsoleWindow();

		/*
		 * Create input devices via gainput
		 */
		void CreateInputDevices();

		/*
		 * Register window class
		 */
		void RegisterWindowClass();

		/*
		 * Directory change listener thread entry point function.
		 */
		void CheckFileChanges();

		/*
		 * Handle to application instance.
		 */
		HINSTANCE m_hInstance;

		Logger m_logger;

		/**
		 * Gainput variables:
		 */
		gainput::InputManager m_inputManager;
		gainput::DeviceId m_keyboardDevice;
		gainput::DeviceId m_mouseDevice;
		gainput::DeviceId m_gamepadDevice[gainput::MaxPadCount];

		/*
		 * Set to true while application is running.
		 */
		std::atomic_bool m_bIsRunning;

		/*
		 * Should application quit?
		 */
		std::atomic_bool m_requestQuit;

		/*
		 * Directory change listener.
		 */
		CReadDirectoryChanges m_directoryChanges;

		/*
		 * Thread to run directory change listener
		 */
		std::thread m_directoryChangeListenerThread;
		std::mutex m_directoryChangeMutex;

		/*
		 * Flag to terminate directory change thread
		 */
		std::atomic_bool m_bTerminateDirectoryChangeThread;
	};

}
