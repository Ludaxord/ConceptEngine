#include "../ConceptEngineRTFramework/Engine/Shared/ConceptEngine.h"

#include <Windows.h>

using namespace Concept::Engine;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ConceptEngine engine = ConceptEngine(hInstance);
	return engine.Run();
}
