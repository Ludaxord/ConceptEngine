// #include "../ConceptEngineFramework/CETools.h"
// #include "../ConceptEngineFramework/ConceptEngine.h"
//
// using namespace ConceptFramework;

#include "../ConceptEngineRTFramework/Engine/Shared/ConceptEngine.h"

#include <Windows.h>

using namespace Concept;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	ConceptEngine engine = ConceptEngine(hInstance);
	return engine.Run();
	return 0;
}
