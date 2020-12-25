#include "CETools.h"

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
