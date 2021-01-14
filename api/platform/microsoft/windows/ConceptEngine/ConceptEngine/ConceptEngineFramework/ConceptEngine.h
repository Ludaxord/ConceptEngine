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
	inline static Logger static_logger_ = nullptr;
	CEOSTools::CEGraphicsApiTypes apiType_;
	CEWindow window_;
	CETimer timer_;
	Logger logger_;

private:
	int g_frameIndx;

};
