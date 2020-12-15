#include "ConceptEngine.h"

ConceptEngine::ConceptEngine() : apiType_(CEOSTools::CEGraphicsApiTypes::direct3d11),
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
		window_.GetGraphics().PrintGraphicsVersion();
	}
}

void ConceptEngine::MakeFrame() {
	const float c1 = sin(timer_.Peek()) / 2.0f + 0.5f;
	const float c2 = cos(timer_.Peek()) / 2.0f + 0.5f;
	const float c3 = sin(-timer_.Peek()) / 2.0f + 0.5f;
	window_.GetGraphics().ClearBuffer(c1, c2, c3);
	const float windowWidth = 800.0f;
	const float windowHeight = 600.0f;
	window_.GetGraphics().DrawDefaultFigure(
		timer_.Peek(),
		windowWidth,
		windowHeight,
		0,
		0,
		0
	);
	window_.GetGraphics().DrawDefaultFigure(
		timer_.Peek(),
		windowWidth,
		windowHeight,
		window_.mouse.GetMousePositionX() / (windowWidth / 2) - 1.0f,
		0,
		-window_.mouse.GetMousePositionY() / (windowHeight / 2) + 1.0f
	);
	window_.GetGraphics().EndFrame();

}
