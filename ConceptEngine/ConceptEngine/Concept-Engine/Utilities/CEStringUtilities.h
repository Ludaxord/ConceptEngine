#pragma once
#include <codecvt>
#include <locale>
#include <string>

inline std::wstring ConvertToWString(const std::string& string) {
	std::wstring wString(string.begin(), string.end());
	return wString;
}

inline std::string ConvertToString(const std::wstring& wString) {
	std::string string(wString.begin(), wString.end());
	return string;
}

inline std::string HResultToString(HRESULT hr) {
	return std::system_category().message(hr);
}

inline void ConvertBackslashes(std::string& OutString) {
	size_t pos = OutString.find_first_of('\\');
	while (pos != std::string::npos) {
		OutString.replace(pos, 1, 1, '/');
		pos = OutString.find_first_of('\\', pos + 1);
	}
}
