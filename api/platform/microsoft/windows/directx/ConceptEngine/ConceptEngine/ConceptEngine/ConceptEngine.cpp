#include "ConceptEngine.h"

#include <ostream>
#include <sstream>

#include "CEConverters.h"

ConceptEngine::ConceptEngine() : window_(800, 600, "Concept Engine Editor", CEWindow::main) {
}

int ConceptEngine::Run() {

	while (true) {
		if (const auto ecode = CEWindow::ProcessMessages()) {
			return *ecode;
		}
		DoFrame();
	}
}

//Game logic per frame
void ConceptEngine::DoFrame() {
	const float c1 = sin(timer_.Peek()) / 2.0f + 0.5f;
	const float c2 = cos(timer_.Peek()) / 2.0f + 0.5f;
	const float c3 = sin(-timer_.Peek()) / 2.0f + 0.5f;
	std::ostringstream oss;
	oss << "color 1: " << c1 << " color 2: " << c2 << " color 3: " << c3 << std::endl;
	OutputDebugString(CEConverters::ConvertCharArrayToLPCWSTR(oss.str().c_str()));
	window_.GetGraphics().ClearBuffer(c1, c2, c3);
	window_.GetGraphics().DrawDefaultTriangle(-timer_.Peek());
	window_.GetGraphics().EndFrame();
}
