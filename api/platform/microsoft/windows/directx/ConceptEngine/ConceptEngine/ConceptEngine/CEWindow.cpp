#include "CEWindow.h"


#include <iostream>
#include <sstream>
#include <string>

#include "CEConverters.h"
#include "CEWindowsMessage.h"

CEWindow::CEWindowClass CEWindow::CEWindowClass::wndClass;
CEWindow::CEWindowTypes CEWindow::CEWindowClass::wndType;

CEWindow::CEWindowClass::CEWindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = {
		sizeof(wc),
		CS_OWNDC,
		HandleMsgSetup,
		0,
		0,
		GetInstance(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		CEConverters::convertCharArrayToLPCWSTR(GetName()),
		nullptr
	};
	RegisterClassEx(&wc);
}

const char* CEWindow::CEWindowClass::GetName() noexcept {
	return wndClassName;
}

CEWindow::CEWindowTypes CEWindow::CEWindowClass::GetWindowType() noexcept {
	return wndType;
}

void CEWindow::CEWindowClass::SetWindowType(CEWindowTypes windowType) noexcept {
	wndType = windowType;
}

HINSTANCE CEWindow::CEWindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}


CEWindow::CEWindowClass::~CEWindowClass() {
	UnregisterClass(CEConverters::convertCharArrayToLPCWSTR(wndClassName), GetInstance());
}

CEWindow::CEWindow(int width, int height, const char* name, CEWindowTypes windowTypes): width(width), height(height) {
	std::ostringstream cen;
	cen << "Initialize Concept Engine Window, Type: " << CEWindowClass::GetWindowType() << " name: " << name << "\n";
	OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(cen.str().c_str()));

	//Start options
	RECT winLoc;
	winLoc.left = 100;
	winLoc.right = width + winLoc.left;
	winLoc.top = 100;
	winLoc.bottom = height + winLoc.top;
	AdjustWindowRect(&winLoc, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	hWnd = CreateWindow(
		CEConverters::convertCharArrayToLPCWSTR(CEWindowClass::GetName()),
		CEConverters::convertCharArrayToLPCWSTR(name),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		winLoc.right - winLoc.left,
		winLoc.bottom - winLoc.top,
		nullptr,
		nullptr,
		CEWindowClass::GetInstance(),
		this //passing CEWindow API function to get in HandleMsgSetup
	);
	CEWindowClass::SetWindowType(windowTypes);
	ShowWindow(hWnd, SW_SHOWDEFAULT);

}

CEWindow::~CEWindow() {
	DestroyWindow(hWnd);
}

LRESULT WINAPI CEWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		CEWindow* const pWnd = static_cast<CEWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)); // creating window

		//passing pointer to HandleMsgThunk to make it main function to call during lifecycle of window (if user press key or close window) 
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CEWindow::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI CEWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	CEWindow* const pWnd = reinterpret_cast<CEWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CEWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {

	//print Windows Message 
	static CEWindowsMessage wm;
	if (CEWindowClass::GetWindowType() == debug) {
		OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(wm(msg, lParam, wParam).c_str()));
	}

	std::ostringstream cen;
	cen << "Concept Engine Window, Type: " << CEWindowClass::GetWindowType() << "\n";
	OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(cen.str().c_str()));

	// switch to pass action to given message
	switch (msg) {
	case WM_CLOSE:
		//TODO: Fix
		if (CEWindowClass::GetWindowType() == main) {
			PostQuitMessage(1);
			return 0;
		}
		else {
			// CloseWindow(hWnd);
			// break;
			PostQuitMessage(2);
			return 0;
		}
	case WM_KEYDOWN:
		//test event
		// if (wParam == 'M') {
		// 	ShowWindow(hWnd, SW_MAXIMIZE);
		// }
		break;
	case WM_CHAR: {
		static std::string title;
		title.push_back((char)wParam);
		OutputDebugString(
			CEConverters::convertCharArrayToLPCWSTR(("==== Keyboard input: " + title + " ====" + "\n").c_str()));
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
