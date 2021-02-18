#pragma once
#include "../RTFramework/DirectXProvider.h"

class D3D12RaytracingLibrarySubObjects : DirectXProvider {
public:
	D3D12RaytracingLibrarySubObjects(): DirectXProvider() {
		OutputDebugStringW(L"D3D12RaytracingLibrarySubObjects test");
	}
	
};
