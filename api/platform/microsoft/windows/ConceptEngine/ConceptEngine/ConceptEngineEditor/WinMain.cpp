#include "../ConceptEngineFramework/CETools.h"
#include "../ConceptEngineFramework/ConceptEngine.h"

using namespace Concept;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	try {
		ConceptEngine engine = ConceptEngine();
		return engine.Run();
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, CETools::ConvertCharArrayToLPCWSTR(e.what()), L"Exception",MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...) {
		MessageBox(nullptr, L"No error details", L"Exception",MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
