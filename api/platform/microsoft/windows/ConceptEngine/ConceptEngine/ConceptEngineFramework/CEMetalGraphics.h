#pragma once
#include "CEGraphics.h"

class CEMetalGraphics : public CEGraphics {
public:
	CEMetalGraphics(HWND hWnd);
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
		CEDefaultFigureTypes figureTypes) override;
};
