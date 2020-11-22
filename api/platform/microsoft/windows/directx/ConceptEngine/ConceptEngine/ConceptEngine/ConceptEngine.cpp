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
	const float c = sin(timer_.Peek()) / 2.0f + 0.5f;
	std::ostringstream oss;
	oss << "color: " << c << std::endl;
	OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
	window_.GetGraphics().ClearBuffer(c, c, 1.0f);
	window_.GetGraphics().EndFrame();
}
