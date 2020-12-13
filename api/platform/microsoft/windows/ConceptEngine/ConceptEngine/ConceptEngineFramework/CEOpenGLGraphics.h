#pragma once
#include "CEGraphics.h"
#include <gl/GL.h>
#include <gl/GLU.h>

class CEOpenGLGraphics : public CEGraphics {
public:
	CEOpenGLGraphics(HWND hWnd);
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;
	CEGraphicsManager GetGraphicsManager() override;
	void Render();

private:
	HDC g_HDC;
	float angle = 0.0f;

};
