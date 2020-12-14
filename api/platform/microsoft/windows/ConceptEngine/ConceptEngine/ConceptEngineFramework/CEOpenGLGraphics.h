#pragma once
#include "CEGraphics.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "OpenGL32.lib")

class CEOpenGLGraphics : public CEGraphics {
public:
	CEOpenGLGraphics(HWND hWnd);
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;
	CEGraphicsManager GetGraphicsManager() override;

	void PrintGraphicsVersion() override;
private:
	HDC g_HDC;
};
