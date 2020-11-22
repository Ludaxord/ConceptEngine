#include "ConceptEngine.h"

#include <iomanip>
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
	const float time = timer_.Peek();
}
