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
	//TODO: Add structure with object that need to appear on the screen on Engine Run;
	//TODO: -> Run() will call LoadContent and init objects that are in passed structure as parameter.
	//TODO: For now objects are created in LoadContent function of every Graphics API
	int Run();
	void MakeFrame();
private:
	CEOSTools::CEGraphicsApiTypes apiType_;
	CEWindow window_;
	CETimer timer_;

private:
	int g_frameIndx;
};
