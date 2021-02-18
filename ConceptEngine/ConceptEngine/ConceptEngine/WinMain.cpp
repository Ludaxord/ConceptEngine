#include <memory>
#include <Windows.h>
#include "../ConceptEngineFramework/CEFramework.h"

using namespace ConceptEngineFramework;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	auto framework = std::make_shared<CEFramework>(L"ConceptEngine", hInstance, lpCmdLine, nCmdShow);
	return framework->Run();
}
