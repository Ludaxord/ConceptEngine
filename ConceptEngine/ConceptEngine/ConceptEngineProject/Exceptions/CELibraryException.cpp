#include "CELibraryException.h"

using namespace ConceptEngine::Project::Exceptions;

CELibraryException::~CELibraryException() {
	LocalFree(sysMessage);
}

DWORD CELibraryException::GetError() const {
	return error;
}

const char* CELibraryException::what() const {
	DWORD sysErr = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		error,
		0,
		(LPSTR)&sysMessage,
		0,
		NULL
	);

	if (sysErr < 1) {
		return "Failed with system error: " + error;
	}

	return sysMessage;
}
