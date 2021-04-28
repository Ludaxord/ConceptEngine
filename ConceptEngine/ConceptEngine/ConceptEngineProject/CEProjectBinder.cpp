#include "CEProjectBinder.h"

#include "Exceptions/CELibraryException.h"

using namespace ConceptEngine::Project;

CEProjectBinder::CEProjectBinder(const TCHAR* ceModule) {
	lnk = LoadLibraryEx(ceModule, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	if (lnk == NULL) {
		throw Exceptions::CELibraryException();
	}
}

CEProjectBinder::~CEProjectBinder() {
	if (lnk != NULL) {
		FreeLibrary(lnk);
	}
}

FARPROC CEProjectBinder::GetProc(LPCSTR lpProcName) {
	auto address = GetProcAddress(lnk, lpProcName);
	if (address == NULL) {
		throw Exceptions::CELibraryException();
	}
	return address;
}
