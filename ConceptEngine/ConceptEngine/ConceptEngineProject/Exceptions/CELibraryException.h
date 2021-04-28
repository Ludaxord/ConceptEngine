#pragma once
#include <concrt.h>
#include <exception>
#include <windows.h>

namespace ConceptEngine::Project::Exceptions {
	class CELibraryException : public std::exception {
	public:
		explicit CELibraryException() : error(GetLastError()), sysMessage(NULL) {

		}

		virtual ~CELibraryException();

		DWORD GetError() const;

		virtual const char* what() const;
	protected:
	private:
		DWORD error;
		LPSTR sysMessage;
	};
}
