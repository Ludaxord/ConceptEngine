#pragma once
#include <Windows.h>


#include "CEOSTools.h"
#include "CETimer.h"
#include "CEWindow.h"

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name);
	ConceptEngine(int width, int height, const char* name, CEOSTools::CEGraphicsApiTypes graphicsApiType);
	int Run();
	void MakeFrame();
private:
	CEWindow window_;
	CETimer timer_;
	CEOSTools::CEGraphicsApiTypes apiType_;
};
