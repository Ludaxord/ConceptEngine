#include <Windows.h>


#include "../DirectXBoxTutorial/DirectXBoxTutorial.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	const std::shared_ptr<DirectXBoxTutorial> tutorial = std::make_shared<DirectXBoxTutorial>(1920, 1080);

	return ConceptEngineRunner::Run(tutorial, hInstance, nCmdShow);
}
