#include "CEOpenGLGraphics.h"
#include "CEOSTools.h"
#include "CETools.h"
#include "CEWindow.h"


CEOpenGLGraphics::CEOpenGLGraphics(HWND hWnd, int width, int height): CEGraphics(
	hWnd, CEOSTools::CEGraphicsApiTypes::opengl, width, height) {

}

void CEOpenGLGraphics::PrintGraphicsVersion() {
	std::wstringstream wss;
	wss << "OpenGL Version: " << CEOSTools::GetOpenGLVersion() << std::endl;
	OutputDebugString(wss.str().c_str());
}

bool CEOpenGLGraphics::OnInit() {
	g_HDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
		PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
		32, // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24, // Number of bits for the depthbuffer
		8, // Number of bits for the stencilbuffer
		0, // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int letWindowsChooseThisPixelFormat;
	letWindowsChooseThisPixelFormat = ChoosePixelFormat(g_HDC, &pfd);

	SetPixelFormat(g_HDC, letWindowsChooseThisPixelFormat, &pfd);

	HGLRC ourOpenGLRenderingContext = wglCreateContext(g_HDC);
	wglMakeCurrent(g_HDC, ourOpenGLRenderingContext);
	return true;
}

void CEOpenGLGraphics::OnDestroy() {
}

void CEOpenGLGraphics::OnRender() {
}

void CEOpenGLGraphics::OnUpdate() {
}

void CEOpenGLGraphics::SetFullscreen(bool fullscreen) {
}

bool CEOpenGLGraphics::LoadContent() {
	return false;
}

void CEOpenGLGraphics::UnloadContent() {
}

void CEOpenGLGraphics::UpdatePerSecond(float second) {
}

CEGraphics::IGPUInfo CEOpenGLGraphics::GetGPUInfo() {
	return IGPUInfo();
}

void CEOpenGLGraphics::OnKeyPressed() {
}

void CEOpenGLGraphics::OnKeyReleased() {
}

void CEOpenGLGraphics::OnMouseMoved() {
}

void CEOpenGLGraphics::OnMouseButtonPressed() {
}

void CEOpenGLGraphics::OnMouseButtonReleased() {
}

void CEOpenGLGraphics::OnMouseWheel() {
}

void CEOpenGLGraphics::OnResize() {
}

void CEOpenGLGraphics::OnWindowDestroy() {
}
