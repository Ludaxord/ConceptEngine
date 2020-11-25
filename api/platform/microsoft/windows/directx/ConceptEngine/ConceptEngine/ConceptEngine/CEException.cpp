#include "CEException.h"
#include <sstream>

#include "CEConverters.h"

CEException::CEException(int exceptionLine, const char* exceptionFile) noexcept:
	exceptionLine(exceptionLine), exceptionFile(exceptionFile) {

}

const char* CEException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* CEException::GetType() const noexcept {
	return "ConceptEngineException";
}

int CEException::GetExceptionLine() const noexcept {
	return exceptionLine;
}

const std::string& CEException::GetExceptionFile() const noexcept {
	return exceptionFile;
}

std::string CEException::GetOriginString() const noexcept {
	std::ostringstream oss;
	oss << "[File] " << exceptionFile << std::endl << "[Line] " << exceptionLine;
	return oss.str();
}
