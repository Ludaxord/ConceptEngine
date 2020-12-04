#include "CEDirect3DGraphics.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType): CEGraphics(hWnd, apiType) {
	if (apiType != CEGraphicsApiTypes::direct3d11 && apiType != CEGraphicsApiTypes::direct3d12) {
		throw Exception(__LINE__, "Wrong Graphics API, this class supports only Direct3D API'S");
	}
}
