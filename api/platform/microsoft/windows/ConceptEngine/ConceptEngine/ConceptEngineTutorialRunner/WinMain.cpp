#include <Windows.h>


#include "../DirectXBoxTutorial/DirectXBoxTutorial.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	std::shared_ptr<DirectXBoxTutorial> tutorial = std::make_shared<DirectXBoxTutorial>(1920, 1080);
	//test logger TODO: Move to ConceptEngineRunner!
	auto logger = tutorial->CreateLogger("Box Tutorial");
	// return ConceptEngineRunner::Run();
	return -1;
}
