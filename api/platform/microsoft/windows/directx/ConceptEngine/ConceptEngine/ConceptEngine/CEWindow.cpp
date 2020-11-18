#include "CEWindow.h"


#include <sstream>
#include <string>

#include "CEConverters.h"
#include "CEWindowsMessage.h"

CEWindow::CEWindowClass CEWindow::CEWindowClass::wndClass;

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

HINSTANCE CEWindow::CEWindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}


CEWindow::CEWindowClass::~CEWindowClass() {
	UnregisterClass(CEConverters::convertCharArrayToLPCWSTR(wndClassName), GetInstance());
}

CEWindow::CEWindow(int width, int height, const char* name) noexcept {
}

CEWindow::~CEWindow() {
	DestroyWindow(hWnd);
}

LRESULT CEWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	if (msg == WM_NCCREATE) {
		//TODO: check reinterpret_cast & static_cast
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		CEWindow* const pWnd = static_cast<CEWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&CEWindow::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CEWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	CEWindow* const pWnd = reinterpret_cast<CEWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CEWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {

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
