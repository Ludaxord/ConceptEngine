#include "../ConceptEngineFramework/CETools.h"
#include "../ConceptEngineFramework/ConceptEngine.h"

using namespace Concept;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	ConceptEngine engine = ConceptEngine(hInstance);
	return engine.Run();

}
