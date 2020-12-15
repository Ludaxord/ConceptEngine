#pragma once
class CETools {
public:
	static wchar_t* ConvertCharArrayToLPCWSTR(const char* charArray);
	static int gcd(int a, int b);
	static void Assert(bool flag, const char* msg);
};
