#pragma once
#include <Windows.h>
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
	Logger CreateLogger(const std::string& name);

private:
	CEOSTools::CEGraphicsApiTypes apiType_;
	CEWindow window_;
	CETimer timer_;
	Logger logger_;

private:
	int g_frameIndx;
};
