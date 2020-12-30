#pragma once
#include <Windows.h>

#include "CEOSTools.h"
#include "CETimer.h"
#include "CEWindow.h"

// using Logger = std::shared_ptr<spdlog::logger>;

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name);
	ConceptEngine(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);
	int Run();
	void MakeFrame();
private:
	CEOSTools::CEGraphicsApiTypes apiType_;
	CEWindow window_;
	CETimer timer_;
	// Logger logger_;

private:
	int g_frameIndx;
};
