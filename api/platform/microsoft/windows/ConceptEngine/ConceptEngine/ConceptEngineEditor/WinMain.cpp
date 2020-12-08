#include "../ConceptEngineFramework/CETools.h"
#include "../ConceptEngineFramework/ConceptEngine.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	try {
		ConceptEngine engine = ConceptEngine();
		return engine.Run();
	}
	catch (const CEException& e) {
		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR(e.what()),
			CETools::ConvertCharArrayToLPCWSTR(e.GetType()),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (const std::exception& e) {
		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR(e.what()),
			CETools::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (...) {
		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR("No error details"),
			CETools::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	return -1;
}
