#include "ConceptEngine.h"

ConceptEngine::ConceptEngine() : apiType_(CEOSTools::CEGraphicsApiTypes::direct3d12),
                                 window_(800, 600, "Concept Engine Editor", apiType_) {

}

ConceptEngine::ConceptEngine(int windowWidth, int windowHeight, const char* windowName) : window_(
	windowWidth, windowHeight, windowName) {

}

ConceptEngine::ConceptEngine(int width, int height, const char* name,
                             CEOSTools::CEGraphicsApiTypes graphicsApiType) : ConceptEngine(width, height, name) {
	apiType_ = graphicsApiType;
}

int ConceptEngine::Run() {

	while (true) {
		if (const auto ecode = CEWindow::ProcessMessages()) {
			return *ecode;
		}

		//debug
		// window_.GetGraphics().PrintGraphicsVersion();
		MakeFrame();
		// g_frameIndx++;
	}
}

void ConceptEngine::MakeFrame() {
	std::wstringstream wssx;
	wssx << "======" << std::endl;
	wssx << "make frame index: " << g_frameIndx << std::endl;
	wssx << "++++++" << std::endl;
	OutputDebugStringW(wssx.str().c_str());
	const float c1 = sin(timer_.Peek()) / 2.0f + 0.5f;
	const float c2 = cos(timer_.Peek()) / 2.0f + 0.5f;
	const float c3 = sin(-timer_.Peek()) / 2.0f + 0.5f;
	window_.GetGraphics().ChangeClearColor(c1, c2, c3);
	window_.GetGraphics().OnUpdate();
	window_.GetGraphics().OnRender();
}
