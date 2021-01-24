#include "CETools.h"


#include <string>
#include <tuple>
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

std::tuple<int, int> CETools::WindowSize(HWND hWnd) {
	RECT rect;
	if (GetWindowRect(hWnd, &rect)) {
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		return std::make_tuple(width, height);
	}
	else {
		return std::make_tuple(0, 0);
	}
}

std::wstring s2ws(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// wide char to multi byte:
std::string ws2s(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), 0, 0, 0, 0);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), int(wstr.length() + 1), &strTo[0], size_needed, 0, 0);
	return strTo;
}
