#pragma once


#include <Windows.h>
#include <fcntl.h>

#include <iostream>
#include <comdef.h>

#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <gainput/gainput.h>


#include "CEInput.h"
#include "CEOSTools.h"
#include "CETimer.h"
#include "CEWindow.h"

using Logger = std::shared_ptr<spdlog::logger>;
using Input = std::shared_ptr<CEInput>;

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name);
	ConceptEngine(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);

	void Init();
	int Run();
	void MakeFrame();

	Logger CreateLogger(const std::string& name) const;
	void InitInput();

	static Input GetInput();
	static Logger GetLogger();

protected:
	void InitSpdLog();
	static void CreateConsoleWindow();


private:
	/*
	 * Global engine state variables:
	 */

	std::atomic_bool isAppRunning_;
	std::atomic_bool appShouldQuit_;

	/*
	 * Input struct variable
	 */
	inline static Input inputs_ = nullptr;
	/*
	 * Global Engine objects variables:
	 */
	inline static Logger static_logger_ = nullptr;
	CEOSTools::CEGraphicsApiTypes apiType_;
	std::unique_ptr<CEWindow> window_ = nullptr;
	CETimer timer_;
	Logger logger_;
};
