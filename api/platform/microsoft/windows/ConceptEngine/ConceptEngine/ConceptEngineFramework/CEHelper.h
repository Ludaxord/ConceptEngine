#pragma once

#define WIN32_LEAN_AND_MEAN
#include <comdef.h>
#include <exception>
#include <Windows.h> // For HRESULT
#include "ConceptEngine.h"

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		_com_error err(hr);
		const auto* errorMessage = err.ErrorMessage();
		OutputDebugString(errorMessage);
		if (ConceptEngine::GetLogger() != nullptr) {
			std::wstringstream wss;
			wss << errorMessage << std::endl;
			// ConceptEngine::GetLogger()->error(wss.str().c_str());
		}
		throw std::exception();
	}
}

inline void GetDesktopResolution(int& horizontal, int& vertical) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

/*
 * Set the name of an std::thread.
 * Useful for debugging.
 */
const DWORD MS_VC_EXCEPTION = 0x406D1388;

/*
 * Set name of running thread
 */
#pragma pack(push, 8)
typedef struct tagTHREAADNAME_INFO {
	/*
	 * Must be 0x1000
	 */
	DWORD dwType;
	/*
	 * Pointer to name
	 */
	LPCSTR szName;
	/*
	 * Thread ID
	 */
	DWORD dwThreadID;
	/*
	 * Reserver for future use, must be zero.
	 */
	DWORD dwFlags;
} THREADNAME_INFO;
#pragma pack(pop)

inline void SetThreadName(std::thread& thread, const char* threadName) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
	info.dwFlags = 0;

	__try {
		::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

/*
 * 
 */

namespace Concept::Math {
	constexpr float PI = 3.1415926535897932384626433832795f;
	constexpr float _2PI = 2.0f * PI;

	/*
	 * Convert radians to degrees.
	 */
	constexpr float Degrees(const float radians) {
		return radians * (180.0f / PI);
	}

	/*
	 * Convert degrees to radians.
	 */
	constexpr float Radians(const float degrees) {
		return degrees * (PI / 180.0f);
	}

	/***************************************************************************
	* These functions were taken from the MiniEngine.
	* Source code available here:
	* https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Math/Common.h
	* Retrieved: January 13, 2016
	**************************************************************************/
	template <typename T>
	inline T AlignUpWithMask(T value, size_t mask) {
		return (T)(((size_t)value + mask) & ~mask);
	}

	template <typename T>
	inline T AlignDownWithMask(T value, size_t mask) {
		return (T)((size_t)value & ~mask);
	}

	template <typename T>
	inline T AlignUp(T value, size_t alignment) {
		return AlignUpWithMask(value, alignment - 1);
	}

	template <typename T>
	inline T AlignDown(T value, size_t alignment) {
		return AlignDownWithMask(value, alignment - 1);
	}

	template <typename T>
	inline bool IsAligned(T value, size_t alignment) {
		return 0 == ((size_t)value & (alignment - 1));
	}

	template <typename T>
	inline T DivideByMultiple(T value, size_t alignment) {
		return (T)((value + alignment - 1) / alignment);
	}


}
