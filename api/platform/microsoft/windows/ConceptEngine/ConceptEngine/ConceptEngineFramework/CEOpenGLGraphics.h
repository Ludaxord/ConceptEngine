#pragma once
#include "CEGraphics.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "OpenGL32.lib")

class CEOpenGLGraphics : public CEGraphics {
public:
	CEOpenGLGraphics(HWND hWnd);
	CEManager GetGraphicsManager() override;

	void PrintGraphicsVersion() override;
	bool OnInit() override;
	void OnDestroy() override;
	void OnRender() override;
	void OnUpdate() override;
	void SetFullscreen(bool fullscreen) override;
private:
	HDC g_HDC;
};
