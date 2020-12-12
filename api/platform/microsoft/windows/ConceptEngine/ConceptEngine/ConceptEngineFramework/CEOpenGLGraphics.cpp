#include "CEOpenGLGraphics.h"

#include "CEOpenGLManager.h"

CEOpenGLGraphics::CEOpenGLGraphics(HWND hWnd): CEGraphics(hWnd, CEGraphicsApiTypes::opengl) {
}

void CEOpenGLGraphics::EndFrame() {
}

void CEOpenGLGraphics::ClearBuffer(float red, float green, float blue, float alpha) noexcept {
}

void CEOpenGLGraphics::DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	CEDefaultFigureTypes figureTypes) {
}

CEGraphicsManager CEOpenGLGraphics::GetGraphicsManager() {
	return static_cast<CEGraphicsManager>(CEOpenGLManager());
}
