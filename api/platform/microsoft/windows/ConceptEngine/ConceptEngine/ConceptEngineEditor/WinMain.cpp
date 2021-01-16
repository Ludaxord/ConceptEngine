#include "../ConceptEngineFramework/CETools.h"
#include "../ConceptEngineFramework/ConceptEngine.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	try {
		ConceptEngine engine = ConceptEngine();
		return engine.Run();
	}
	catch (const CEException& e) {
		std::wstringstream wssx;
		wssx << "const CEException: " << e.what() << std::endl;
		OutputDebugStringW(wssx.str().c_str());

		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR(e.what()),
			CETools::ConvertCharArrayToLPCWSTR(e.GetType()),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (const std::exception& e) {
		std::wstringstream wssx;
		wssx << "std::exception: " << e.what() << " FILE: " << __FILE__ << " LINE: " << __LINE__ << std::endl;
		OutputDebugStringW(wssx.str().c_str());

		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR(e.what()),
			CETools::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (...) {
		std::wstringstream wssx;
		wssx << "...: " << "No error details" << std::endl;
		OutputDebugStringW(wssx.str().c_str());

		MessageBox(
			nullptr,
			CETools::ConvertCharArrayToLPCWSTR("No error details"),
			CETools::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	return -1;
}
