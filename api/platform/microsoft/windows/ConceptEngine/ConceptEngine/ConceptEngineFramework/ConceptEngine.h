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
// #include <gainput/gainput.h>

#include "CEOSTools.h"
#include "CETimer.h"
#include "CEWindow.h"

using Logger = std::shared_ptr<spdlog::logger>;

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name);
	ConceptEngine(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);
	int Run();
	void MakeFrame();
public:
	Logger CreateLogger(const std::string& name) const;
	static Logger GetLogger();

protected:
	void InitSpdLog();
	static void CreateConsole();


private:
	/*
	 * Global engine state variables:
	 */

	std::atomic_bool isAppRunning_;
	std::atomic_bool appShouldQuit_;

	/*
	 * Gainput variables:
	 */
	// gainput::InputManager inputManager_;
	// gainput::DeviceId keyboardDevice_;
	// gainput::DeviceId mouseDevice_;
	// gainput::DeviceId gamePadDevice_[gainput::MaxPadCount];
	
	/*
	 * Global Engine objects variables:
	 */
	inline static Logger static_logger_ = nullptr;
	CEOSTools::CEGraphicsApiTypes apiType_;
	CEWindow window_;
	CETimer timer_;
	Logger logger_;
};
