#include "CEDirect3DGraphics.h"

#include <iostream>

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType): CEGraphics(hWnd, apiType) {
	if (apiType != CEGraphicsApiTypes::direct3d11 && apiType != CEGraphicsApiTypes::direct3d12) {
		throw Exception(__LINE__, "Wrong Graphics API, this class supports only Direct3D API'S");
	}
}

std::wstring CEDirect3DGraphics::GetShadersPath(LPCWSTR fileName) {
	WCHAR assetsPath[512];
	DWORD size = GetModuleFileName(nullptr, assetsPath, _countof(assetsPath));
	if (size == 0 || size == _countof(assetsPath)) {
		// Method failed or path was truncated.
		throw std::exception();
	}
	std::wstringstream wss;

	WCHAR* lastSlash = wcsrchr(assetsPath, L'\\');
	wss << "LAST SLASH: " << lastSlash << std::endl;
	if (lastSlash) {
		*(lastSlash - 5) = L'\0';
	}
	std::wstring m_assetsPath = assetsPath;
	m_assetsPath += L"ConceptEngine\\";
	wss << "PATH: " << m_assetsPath + fileName << std::endl;
	OutputDebugString(wss.str().c_str());
	return m_assetsPath + fileName;
}
