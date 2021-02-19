#pragma once
#include <Windows.h> // For HRESULT
#include <fcntl.h>

#include <iostream>
#include <comdef.h>
#include <spdlog/spdlog.h>

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		std::string ss(s_str);
		spdlog::error(ss.c_str());
	}
}
