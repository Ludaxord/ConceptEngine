#pragma once
#include <activation.h>
#include <exception>
#include <spdlog/spdlog.h>

class HrException : public std::runtime_error {
	inline std::string HrToString(HRESULT hr) {
		char s_string[64] = {};
		sprintf_s(s_string, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_string);
	}

public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {
		spdlog::error(HrToString(hr));
	}

	HRESULT Error() const { return m_hr; }
protected:
private:
	const HRESULT m_hr;
};

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize) {
	if (path == nullptr) {
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize) {
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash) {
		*(lastSlash + 1) = L'\0';
	}
};

inline void ThrowIfFailed(HRESULT hr, const wchar_t* message) {
	if (FAILED(hr)) {
		OutputDebugString(message);
		spdlog::error(message);
		throw HrException(hr);
	}
}

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw HrException(hr);
	}
}

inline void ThrowIfFalse(bool value) {
	ThrowIfFailed(value ? S_OK : E_FAIL);
}

inline void ThrowIfFalse(bool value, const wchar_t* message) {
	ThrowIfFailed(value ? S_OK : E_FAIL, message);
}
