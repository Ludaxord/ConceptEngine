#include "CEConverters.h"

#include <Windows.h>


wchar_t* CEConverters::ConvertCharArrayToLPCWSTR(const char* charArray) {
	const auto w_string = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, w_string, 4096);
	return w_string;
}

int CEConverters::gcd(int a, int b) {
	return (b == 0) ? a : gcd(b, a % b);
}
