#pragma once
#include "CEGraphics.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma comment(lib, "OpenGL32.lib")

class CEOpenGLGraphics : public CEGraphics {
public:
	CEOpenGLGraphics(HWND hWnd, int width, int height);

	bool OnInit() override;
	void OnDestroy() override;
	void OnRender() override;
	void OnUpdate() override;
	void SetFullscreen(bool fullscreen) override;
	bool LoadContent() override;
	void UnloadContent() override;
	void UpdatePerSecond(float second) override;
protected:
	void OnKeyPressed() override;
	void OnKeyReleased() override;
	void OnMouseMoved() override;
	void OnMouseButtonPressed() override;
	void OnMouseButtonReleased() override;
	void OnMouseWheel() override;
	void OnResize() override;
	void OnWindowDestroy() override;
public:
	virtual void LogSystemInfo() override;
protected:
	void InitGui() override;
	void RenderGui() override;
	void DestroyGui() override;
private:
	HDC g_HDC;
};
