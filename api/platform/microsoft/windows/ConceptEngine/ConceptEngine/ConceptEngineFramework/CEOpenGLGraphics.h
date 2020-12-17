#pragma once
#include "CEGraphics.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "OpenGL32.lib")

class CEOpenGLGraphics : public CEGraphics {
public:
	CEOpenGLGraphics(HWND hWnd);
	CEGraphicsManager GetGraphicsManager() override;

	void PrintGraphicsVersion() override;
private:
	HDC g_HDC;
};
