#include <Windows.h>
#include "Converters.cpp"
#include "WindowsMessage.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//print Windows Message 
	static WindowsMessage wm;
	OutputDebugString(convertCharArrayToLPCWSTR(wm(msg, lParam, wParam).c_str()));
	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	case WM_KEYDOWN:
		if (wParam == 'M') {
			ShowWindow(hWnd, SW_MAXIMIZE);
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	auto* const p_class_name = convertCharArrayToLPCWSTR("ConceptEngine");
	auto* const p_window_name = convertCharArrayToLPCWSTR("Concept Engine Editor");

	// register window class
	WNDCLASSEX wc = {
		sizeof(wc),
		CS_OWNDC,
		WndProc,
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

	//create event
	// create message
	MSG msg;
	BOOL result;
	while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (result == -1) {
		return -1;
	}

	return msg.wParam;
}
