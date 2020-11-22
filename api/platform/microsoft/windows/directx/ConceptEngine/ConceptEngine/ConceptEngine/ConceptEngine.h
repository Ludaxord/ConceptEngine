#pragma once
#include "CEWindow.h"
#include "CETimer.h"

class ConceptEngine {
public:
	ConceptEngine();
	int Run();
private:
	void DoFrame();
private:
	CEWindow window_;
	CETimer timer_;
};
