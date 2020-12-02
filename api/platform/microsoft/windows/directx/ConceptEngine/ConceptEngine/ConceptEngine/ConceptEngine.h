#pragma once
#include "CEWindow.h"
#include "CETimer.h"

class ConceptEngine {
public:
	ConceptEngine();
	ConceptEngine(int width, int height, const char* name, CEWindow::CEWindowTypes type);
	int Run();
private:
	void DoFrame();
private:
	CEWindow window_;
	CETimer timer_;
};
