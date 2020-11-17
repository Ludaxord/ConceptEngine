#include <Windows.h>
#include "Converters.cpp"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	auto* const p_class_name = convertCharArrayToLPCWSTR("ConceptEngine");
	auto* const p_window_name = convertCharArrayToLPCWSTR("Concept Engine Editor");

	// register window class
	WNDCLASSEX wc = {
		sizeof(wc),
		CS_OWNDC,
		DefWindowProc,
		0,
		0,
		hInstance,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		p_class_name,
		nullptr
	};
	RegisterClassEx(&wc);

	// create window instance (basic window, change to full screen later) TODO: Check how to determine user screen resolution 
	HWND hWnd = CreateWindowEx(
		0,
		p_class_name,
		p_window_name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200,
		200,
		640,
		480,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);
	ShowWindow(hWnd, SW_SHOW);
	return 0;
}
