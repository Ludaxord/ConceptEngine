#include "CEWindow.h"

#include "CEGame.h"
#include <Windows.h>

#include <spdlog/spdlog.h>


using namespace ConceptEngineFramework::Game;

CEWindow::CEWindow(std::wstring windowName, HINSTANCE hInstance, int width, int height):
	m_windowName(windowName),
	m_hInstance(hInstance),
	m_width(width),
	m_height(height) {
	std::wstring prefixName = L"Main";
	std::wstringstream wss;
	wss << prefixName << m_windowName;
	m_windowClassName = wss.str();
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return CEGame::Get().MsgProc(hwnd, msg, wParam, lParam);
}

bool CEWindow::RegisterWindow() const {
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		_com_error err(hr);
		std::wstring werrorMessage(err.ErrorMessage());
		std::string errorMessage(werrorMessage.begin(), werrorMessage.end());
		spdlog::critical("CoInitialize failed: {}", errorMessage);
		throw new std::exception(errorMessage.c_str());
	}

	WNDCLASSEXW wndClass = {0};

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &MainWndProc;
	wndClass.hInstance = m_hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = static_cast<HICON>(LoadImage(m_hInstance, nullptr, IMAGE_ICON, 32, 32, 0));
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = m_windowClassName.c_str();
	wndClass.hIconSm = static_cast<HICON>(LoadImage(m_hInstance, nullptr, IMAGE_ICON, 16, 16, 0));

	if (!RegisterClassExW(&wndClass)) {
		MessageBoxA(NULL, "Unable to register window class.", "Error", MB_OK | MB_ICONERROR);
	}

	return true;
}

bool CEWindow::InitWindow() {

	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = {0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height)};

	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	uint32_t width = windowRect.right - windowRect.left;
	uint32_t height = windowRect.bottom - windowRect.top;

	int windowX = std::max<int>(0, (screenWidth - (int)width) / 2);
	int windowY = std::max<int>(0, (screenHeight - (int)height) / 2);

	m_hWnd = ::CreateWindowExW(NULL,
	                           m_windowClassName.c_str(),
	                           m_windowName.c_str(),
	                           WS_OVERLAPPEDWINDOW, windowX,
	                           windowY,
	                           width,
	                           height,
	                           NULL,
	                           NULL,
	                           m_hInstance,
	                           NULL);
	if (!m_hWnd) {
		std::wstring message = L"Create Window Class Failed";
		std::string sMessage(message.begin(), message.end());
		spdlog::error(sMessage);
		MessageBox(0, message.c_str(), 0, 0);
		return false;
	}

	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);

	return true;
}

std::wstring CEWindow::GetName() {
	return m_windowName;
}

void CEWindow::SetResolution(int width, int height) {
	SetWidth(width);
	SetHeight(height);
}

void CEWindow::SetWidth(int width) {
	m_width = width;
}

void CEWindow::SetHeight(int height) {
	m_height = height;
}

void CEWindow::Create() {
	if (!RegisterWindow()) {
		spdlog::error("Register Concept Engine Window Failed");
	}
}

void CEWindow::Destroy() {
}

void CEWindow::Show() {
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	spdlog::info("Window Shown");
}

void CEWindow::Hide() {
	spdlog::info("Window Hidden");
}
