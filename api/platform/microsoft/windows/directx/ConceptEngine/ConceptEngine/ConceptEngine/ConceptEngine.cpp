#include "ConceptEngine.h"

ConceptEngine::ConceptEngine() : window_(800, 600, "Concept Engine Editor", CEWindow::main) {
}

int ConceptEngine::Run() {
	MSG msg;
	BOOL result;

	while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		//TranslateMessage used to use passed input ex. Values of key press on keyboard. That Used WM_CHAR from MSG object.
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		DoFrame();
	}

	if (result == -1) {
		return -1;
	}

	return msg.wParam;
}

//Game logic
void ConceptEngine::DoFrame() {
	
}
