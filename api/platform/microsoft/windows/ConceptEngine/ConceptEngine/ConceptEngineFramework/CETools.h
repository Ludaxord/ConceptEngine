#pragma once
#include <activation.h>
#include <tuple>

class CETools {
public:
	static wchar_t* ConvertCharArrayToLPCWSTR(const char* charArray);
	static int gcd(int a, int b);
	static void Assert(bool flag, const char* msg);
	static std::tuple<int, int> WindowSize(HWND hWnd);

	static void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize) {
		if (path == nullptr) {
			throw std::exception();
		}

		DWORD size = GetModuleFileName(nullptr, path, pathSize);
		if (size == 0 || size == pathSize) {
			// Method failed or path was truncated.
			throw std::exception();
		}

		WCHAR* lastSlash = wcsrchr(path, L'\\');
		if (lastSlash) {
			*(lastSlash + 1) = L'\0';
		}
	}
};
