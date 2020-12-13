#include "CEOpenGLGraphics.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "CEOpenGLManager.h"
#include "CEOSTools.h"

CEOpenGLGraphics::CEOpenGLGraphics(HWND hWnd): CEGraphics(hWnd, CEOSTools::CEGraphicsApiTypes::opengl) {
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

void CEOpenGLGraphics::Render() {
	/*      Enable depth testing
	*/
	glEnable(GL_DEPTH_TEST);

	/*      Heres our rendering. Clears the screen
	        to black, clear the color and depth
	        buffers, and reset our modelview matrix.
	*/
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/*      Increase rotation angle counter
	*/
	angle = angle + 1.0f;

	/*      Reset after we have completed a circle
	*/
	if (angle >= 360.0f) {
		angle = 0.0f;
	}

	glPushMatrix();
	glLoadIdentity();

	/*      Move to 0,0,-30 , rotate the robot on
	        its y axis, draw the robot, and dispose
	        of the current matrix.
	*/
	glTranslatef(0.0f, 0.0f, -30.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glPopMatrix();

	glFlush();

	/*      Bring back buffer to foreground
	*/
	SwapBuffers(g_HDC);

}
