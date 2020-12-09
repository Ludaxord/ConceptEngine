#include "CEDirect3DGraphics.h"

CEDirect3DGraphics::CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType): CEGraphics(hWnd, apiType) {
	if (apiType == CEGraphicsApiTypes::direct3d11) {

	}
	else if (apiType == CEGraphicsApiTypes::direct3d12) {

	}
	else {
		throw CEException(__LINE__, __FILE__);
	}
}

void CEDirect3DGraphics::EndFrame() {
#ifndef NDEBUG
	infoManager.Set();
#endif
}

void CEDirect3DGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEDirect3DGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y,
                                           float z, CEDefaultFigureTypes figureTypes) {
}
