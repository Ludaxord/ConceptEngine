// #include "../ConceptEngineRTFramework/Engine/Shared/ConceptEngine.h"
#include "../ConceptEngineFramework/ConceptEngine.h"

#include <Windows.h>

// using namespace Concept::Engine;
using namespace ConceptFramework;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ConceptEngine engine = ConceptEngine(hInstance);
	return engine.Run();
}
