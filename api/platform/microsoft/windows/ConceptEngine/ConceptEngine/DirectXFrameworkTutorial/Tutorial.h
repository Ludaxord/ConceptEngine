#pragma once
#include <activation.h>
#include <dxgi.h>
#include <intsafe.h>
#include <iostream>
#include <memory>
#include <string>
#include <wrl.h>
#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "DirectXHelper.h"

#include "DirectXResources.h"
#include "KeyCode.h"
#include "StepTimer.h"
#include <fcntl.h> // For Translation mode constants _O_TEXT (https://docs.microsoft.com/en-us/cpp/c-runtime-library/translation-mode-constants)

#include "ConceptEngineRunner.h"

namespace wrl = Microsoft::WRL;

class Tutorial {
public:
	using Logger = std::shared_ptr<spdlog::logger>;

	inline static UINT MAX_CONSOLE_LINES = 500;

	Tutorial(UINT width, UINT height, std::wstring name): m_width(width),
	                                                      m_height(height),
	                                                      m_windowBounds{0, 0, 0, 0},
	                                                      m_title(name),
	                                                      m_aspectRatio(0.0f),
	                                                      m_enableUI(true),
	                                                      m_adapterID(0),
	                                                      m_vSync(false) {
		WCHAR assetsPath[512];
		GetAssetsPath(assetsPath, _countof(assetsPath));
		m_assetsPath = assetsPath;
		CreateConsoleWindow();
		m_logger->info("Window created");
		UpdateForSizeChange(width, height);
	}

	virtual ~Tutorial() = default;

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnSizeChanged(UINT width, UINT height, bool minimized) = 0;

	virtual void OnKeyDown(KeyCode key) = 0;
	virtual void OnKeyUp(KeyCode key) = 0;
	virtual void OnWindowMoved(int x, int y) = 0;
	virtual void OnMouseMove(UINT x, UINT y) = 0;

	virtual void OnMouseButtonDown(KeyCode key, UINT x, UINT y) = 0;
	virtual void OnMouseButtonUp(KeyCode key, UINT x, UINT y) = 0;
	virtual void OnDisplayChanged() = 0;

	virtual void OnDeviceLost() {
		ReleaseWindowSizeDependentResources();
		ReleaseDeviceDependentResources();
	}

	virtual void OnDeviceRestored() {
		CreateDeviceDependentResources();
		CreateWindowSizeDependentResources();
	}

	virtual void OnDestroy() {

	}

	virtual void CreateDeviceDependentResources() = 0;
	virtual void CreateWindowSizeDependentResources() = 0;
	virtual void ReleaseDeviceDependentResources() = 0;
	virtual void ReleaseWindowSizeDependentResources() = 0;

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


	/**
	 * Create loggers
	 * @see https://github.com/gabime/spdlog#asynchronous-logger-with-multi-sinks
	 */
	Logger CreateLogger(const std::string& name) const {
		Logger logger = spdlog::get(name);
		if (!logger) {
			logger = m_logger->clone(name);
			spdlog::register_logger(logger);
		}
		return logger;
	}

	void CreateConsoleWindow() {
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

	std::wstring GetTitle() const { return m_title; }
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }
	RECT GetWindowsBounds() const { return m_windowBounds; }
	DirectXResources* GetDirectXResources() const { return m_deviceResources.get(); }

	void UpdateForSizeChange(UINT clientWidth, UINT clientHeight) {
		m_width = clientWidth;
		m_height = clientHeight;
		m_aspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
	}

	void SetWindowBounds(int left, int top, int right, int bottom) {
		m_windowBounds = {
			static_cast<LONG>(left),
			static_cast<LONG>(top),
			static_cast<LONG>(right),
			static_cast<LONG>(bottom)
		};
	}

	std::wstring GetAssetFullPath(LPCWSTR assetName) const {
		return m_assetsPath + assetName;
	}

	virtual wrl::ComPtr<IDXGISwapChain> GetSwapChain() {
		return nullptr;
	}

protected:
	void SetCustomWindowText(LPCWSTR text) const {
		std::wstring windowText = m_title + L": " + text;
		SetWindowText(ConceptEngineRunner::GetHWnd(), windowText.c_str());
	}

	static const UINT FrameCount = 3;

	//Viewport Dimensions
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	//Window bounds
	RECT m_windowBounds;

	bool m_enableUI;

	//DirectX resources
	UINT m_adapterID;
	bool m_vSync;
	std::unique_ptr<DirectXResources> m_deviceResources;

	//Application state
	StepTimer m_timer;
	Logger m_logger;
private:
	//Root assets path
	std::wstring m_assetsPath;

	//Window Title
	std::wstring m_title;
};
