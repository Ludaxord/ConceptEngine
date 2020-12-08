#pragma once
#include <exception>
#include <string>
// #include "CEWindow.h"


class CEException : public std::exception {
public:
	CEException(int exceptionLine, const char* exceptionFile) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetExceptionLine() const noexcept;
	const std::string& GetExceptionFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int exceptionLine;
	std::string exceptionFile;
protected:
	mutable std::string whatBuffer;
};
