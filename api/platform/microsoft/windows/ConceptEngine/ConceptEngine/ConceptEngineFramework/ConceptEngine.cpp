#include "ConceptEngine.h"

#include <sstream>

#include "CETools.h"

ConceptEngine::ConceptEngine() {
	WelcomeMessage();
}

int ConceptEngine::Run() {

	while (true) {
		if (const auto ecode = CEWindow::ProcessMessages()) {
			return *ecode;
		}
	}
}

void ConceptEngine::WelcomeMessage() {
	std::ostringstream oss;
	oss << "Welcome to Concept Engine" << std::endl;
	OutputDebugString(CETools::ConvertCharArrayToLPCWSTR(oss.str().c_str()));
}
