#pragma once
#include <Windows.h>


#include "CETimer.h"
#include "CEWindow.h"

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name);
	ConceptEngine(int width, int height, const char* name, CEGraphics::CEGraphicsApiTypes graphicsApiType);
	int Run();
	void MakeFrame();
private:
	CEWindow window_;
	CETimer timer_;
	CEGraphics::CEGraphicsApiTypes apiType_;
};
