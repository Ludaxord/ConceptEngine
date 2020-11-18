#include <sstream>
#include <Windows.h>
#include "CEConverters.h"
#include "CEWindowsMessage.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//print Windows Message 
	static CEWindowsMessage wm;
	OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(wm(msg, lParam, wParam).c_str()));
	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
	case WM_KEYDOWN:
		//test event
		// if (wParam == 'M') {
		// 	ShowWindow(hWnd, SW_MAXIMIZE);
		// }
		break;
	case WM_CHAR: {
		static std::string title;
		title.push_back((char)wParam);
		OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(("==== Keyboard input: " + title + " ====" + "\n").c_str()));
	}
	break;
	case WM_LBUTTONDOWN: {
		//coordinates of mouse 
		const auto pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "Mouse coordinates: " << "(" << pt.x << ", " << pt.y << ")" << "\n";
		OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
	}
	break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//TODO: Call CEWindow instead of manually create window with functions!
	auto* const p_class_name = CEConverters::convertCharArrayToLPCWSTR("ConceptEngine");
	auto* const p_window_name = CEConverters::convertCharArrayToLPCWSTR("Concept Engine Editor");

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
		//TranslateMessage used to use passed input ex. Values of key press on keyboard. That Used WM_CHAR from MSG object.
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (result == -1) {
		return -1;
	}

	return msg.wParam;
}
