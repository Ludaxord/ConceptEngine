#include "CEWindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	CEWindow ceWindow(800, 300, "Concept Engine Editor", CEWindow::main);
	CEWindow ceWindowAdditional(300, 800, "Concept Engine Tools", CEWindow::additional);

	// create message
	MSG msg;
	BOOL result;
	while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		//TranslateMessage used to use passed input ex. Values of key press on keyboard. That Used WM_CHAR from MSG object.
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (result == -1) {
		return -1;
	}

	return msg.wParam;
}
