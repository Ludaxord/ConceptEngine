#include "CEMetalGraphics.h"

CEMetalGraphics::CEMetalGraphics(HWND hWnd): CEGraphics(hWnd, CEGraphicsApiTypes::metal) {
}

void CEMetalGraphics::EndFrame() {
}

void CEMetalGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEMetalGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	CEDefaultFigureTypes figureTypes) {
}
