#pragma once
#include "CEWindow.h"

class ConceptEngine {
public:
	ConceptEngine();
	int Run();
private:
	void DoFrame();
private:
	CEWindow window_;
};
