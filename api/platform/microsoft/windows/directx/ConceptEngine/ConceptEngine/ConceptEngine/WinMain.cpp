#include <ostream>


#include "CEConverters.h"
#include "CEWindow.h"
#include "ConceptEngine.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	try {
		auto engine = ConceptEngine();
		return engine.Run();
	}
	catch (const CEException& e) {
		// CEException::CEExceptionBox(nullptr, e.what(), e.GetType(),MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::ConvertCharArrayToLPCWSTR(e.what()),
			CEConverters::ConvertCharArrayToLPCWSTR(e.GetType()),
			MB_OK | MB_ICONEXCLAMATION
		);
	} catch (const std::exception& e) {
		// CEException::CEExceptionBox(nullptr, e.what(), "Exception", MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::ConvertCharArrayToLPCWSTR(e.what()),
			CEConverters::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	} catch (...) {
		// CEException::CEExceptionBox(nullptr, "No error details", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::ConvertCharArrayToLPCWSTR("No error details"),
			CEConverters::ConvertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	return -1;
}
