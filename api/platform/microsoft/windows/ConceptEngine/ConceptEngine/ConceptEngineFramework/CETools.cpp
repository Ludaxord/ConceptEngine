#include "CETools.h"
#include <Windows.h>

wchar_t* CETools::ConvertCharArrayToLPCWSTR(const char* charArray) {
	auto* const w_string = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, w_string, 4096);
	return w_string;
}

int CETools::gcd(int a, int b) {
	return (b == 0) ? a : gcd(b, a % b);
}

void CETools::Assert(bool flag, const char* msg) {
	if (!flag) {
		OutputDebugStringA("ASSERT: ");
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
		int* base = 0;
		*base = 1;
	}
}
