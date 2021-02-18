#include "CEConsole.h"


#include <fcntl.h>
#include <iostream>
#include <spdlog/spdlog.h>
// STL
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../CEHeaderLibs.h"

using namespace ConceptEngineFramework::Game;


CEConsole::CEConsole(std::wstring windowName, int maxFileSizeInMB, int maxFiles, int maxConsoleLines):
	m_windowName(windowName),
	m_maxFileSizeInMB(maxFileSizeInMB),
	m_maxFiles(maxFiles),
	m_maxConsoleLines(maxConsoleLines) {
}

void CEConsole::Create() {
	RegisterConsole();
	RegisterLogger();
}

void CEConsole::Destroy() {
}

void CEConsole::Show() {
}

void CEConsole::Hide() {
}

void CEConsole::RegisterLogger() {
	spdlog::init_thread_pool(8192, 1);
	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"logs/log.txt", 1024 * 1024 * m_maxFileSizeInMB, m_maxFiles, true);
	// Max size: 5MB, Max files: 3, Rotate on open: true
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink, msvc_sink};

	std::string windowName(m_windowName.begin(), m_windowName.end());
	m_logger = std::make_shared<spdlog::async_logger>(windowName, sinks.begin(), sinks.end(),
	                                                  spdlog::thread_pool(), spdlog::async_overflow_policy::block);
	spdlog::register_logger(m_logger);
	spdlog::set_default_logger(m_logger);
	spdlog::flush_on(spdlog::level::info);
}

void CEConsole::RegisterConsole() {
	// Allocate a console.
	if (AllocConsole()) {
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Increase screen buffer to allow more lines of text than the default.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
		consoleInfo.dwSize.Y = m_maxConsoleLines;
		
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
