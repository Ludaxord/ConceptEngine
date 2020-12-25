#pragma once
#include <activation.h>
#include <tuple>

class CETools {
public:
	static wchar_t* ConvertCharArrayToLPCWSTR(const char* charArray);
	static int gcd(int a, int b);
	static void Assert(bool flag, const char* msg);
	static std::tuple<int, int> WindowSize(HWND hWnd);
};
