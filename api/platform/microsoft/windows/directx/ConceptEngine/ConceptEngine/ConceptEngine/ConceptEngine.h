#pragma once
#include "CETimer.h"
#include "CEWindow.h"

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
