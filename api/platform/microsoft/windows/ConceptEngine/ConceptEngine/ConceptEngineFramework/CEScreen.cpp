#include "CEScreen.h"

#include "CEWinLib.h"

using namespace Concept::GameEngine;

CEScreen::CEScreen(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight)
	: m_hWnd(hWnd),
	  m_name(windowName),
	  m_title(windowName),
	  m_clientWidth(clientWidth),
	  m_clientHeight(clientHeight),
	  m_previousMouseX(0),
	  m_previousMouseY(0),
	  m_isFullscreen(false),
	  m_isMinimized(false),
	  m_isMaximized(false),
	  m_bInClientRect(false),
	  m_bHasKeyboardFocus(false) {
	m_DPIScaling = ::GetDpiForWindow(hWnd) / 96.0f;
}

CEScreen::~CEScreen() {
	::DestroyWindow(m_hWnd);
}

HWND CEScreen::GetWindowHandle() const {
	return m_hWnd;
}

float CEScreen::GetDPIScaling() const {
	return m_DPIScaling;
}

const std::wstring& CEScreen::GetWindowName() const {
	return m_name;
}

void CEScreen::SetWindowTitle(const std::wstring& windowTitle) {
	m_title = windowTitle;
	::SetWindowTextW(m_hWnd, m_title.c_str());
}

const std::wstring& CEScreen::GetWindowTitle() const {
	return m_title;
}

bool CEScreen::IsFullscreen() const {
	return m_isFullscreen;
}

bool CEScreen::IsCursorVisible() const {
	return m_cursorVisible;
}

void CEScreen::SetFullscreen(bool fullscreen) {
	if (m_isFullscreen != fullscreen) {
		m_isFullscreen = fullscreen;

		/*
		 * Switching to fullscreen
		 */
		if (m_isFullscreen) {
			/*
			 * Store current window dimensions so they can be restored
			 * when switching out of fullscreen state.
			 */
			::GetWindowRect(m_hWnd, &m_windowRect);

			/*
			 * Set window style to borderless fullscreen window so client area fills entire screen.
			 */
			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX |
				WS_MAXIMIZEBOX);

			::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

			/*
			 * Query name of nearest display device for window,
			 * This is required to set fullscreen dimensions of window
			 * when using multi-monitor setup.
			 */
			HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(m_hWnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			               monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
			               monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_FRAMECHANGED | SWP_NOACTIVATE);
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		}
		else {
			/*
			 * Restore all window decorators.
			 */
			::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			::SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_windowRect.left, m_windowRect.top,
			               m_windowRect.right - m_windowRect.left, m_windowRect.bottom - m_windowRect.top,
			               SWP_FRAMECHANGED | SWP_NOACTIVATE);
			::ShowWindow(m_hWnd, SW_NORMAL);
			SetCursor(true);
		}
	}
}

void CEScreen::ToggleCursor() {
	SetCursor(!m_cursorVisible);
}

void CEScreen::SetCursor(bool cursorVisible) {
	if (m_cursorVisible != cursorVisible) {
		m_cursorVisible = cursorVisible;
		ShowCursor(m_cursorVisible);
	}
}

void CEScreen::ToggleFullscreen() {
	SetFullscreen(!m_isFullscreen);
}

void CEScreen::Show() {
	::ShowWindow(m_hWnd, SW_SHOW);
}

void CEScreen::Hide() {
	::ShowWindow(m_hWnd, SW_HIDE);
}

void CEScreen::OnUpdate(UpdateEventArgs& e) {
	m_timer.Tick();

	e.DeltaTime = m_timer.ElapsedSeconds();
	e.TotalTime = m_timer.TotalSeconds();

	Update(e);
}

/*
 * DPI scaling of window has changed
 */
void CEScreen::OnDPIScaleChanged(DPIScaleEventArgs& e) {
	m_DPIScaling = e.DPIScale;
	DPIScaleChanged(e);
}

void CEScreen::OnClose(WindowCloseEventArgs& e) {
	Close(e);
}

void CEScreen::OnResize(ResizeEventArgs& e) {
	m_clientWidth = e.Width;
	m_clientHeight = e.Height;
	if ((m_isMinimized || m_isMaximized) && e.State == WindowState::Restored) {
		m_isMaximized = false;
		m_isMinimized = false;
		OnRestored(e);
	}

	Resize(e);
}

void CEScreen::OnMinimized(ResizeEventArgs& e) {
	Minimized(e);
}

void CEScreen::OnMaximized(ResizeEventArgs& e) {
	Maximized(e);
}

void CEScreen::OnRestored(ResizeEventArgs& e) {
	Restored(e);
}

/*
 * Keyboard key was pressed
 */
void CEScreen::OnKeyPressed(KeyEventArgs& e) {
	KeyPressed(e);
}

/*
 * Keyboard key was released
 */
void CEScreen::OnKeyReleased(KeyEventArgs& e) {
	KeyReleased(e);
}

/*
 * Window gained keyboard focus
 */
void CEScreen::OnKeyboardFocus(EventArgs& e) {
	m_bHasKeyboardFocus = true;
	KeyboardFocus(e);
}

void CEScreen::OnKeyboardBlur(EventArgs& e) {
	m_bHasKeyboardFocus = false;
	KeyboardBlur(e);
}

void CEScreen::OnMouseMoved(MouseMotionEventArgs& e) {
	if (!m_bInClientRect) {
		m_previousMouseX = e.X;
		m_previousMouseY = e.Y;
		m_bInClientRect = true;
		/*
		 * Mouse re-entered client area.
		 */
		OnMouseEnter(e);
	}

	e.RelX = e.X - m_previousMouseX;
	e.RelY = e.Y - m_previousMouseY;

	m_previousMouseX = e.X;
	m_previousMouseY = e.Y;

	MouseMoved(e);
}

void CEScreen::OnMouseButtonPressed(MouseButtonEventArgs& e) {
	MouseButtonPressed(e);
}

void CEScreen::OnMouseButtonReleased(MouseButtonEventArgs& e) {
	MouseButtonReleased(e);
}

void CEScreen::OnMouseWheel(MouseWheelEventArgs& e) {
	MouseWheel(e);
}

void CEScreen::OnMouseEnter(MouseMotionEventArgs& e) {
	/*
	 * Track mouse leave events
	 */
	TRACKMOUSEEVENT trackMouseEvent = {};
	trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	trackMouseEvent.hwndTrack = m_hWnd;
	trackMouseEvent.dwFlags = TME_LEAVE;
	TrackMouseEvent(&trackMouseEvent);

	m_bInClientRect = true;
	MouseEnter(e);
}

void CEScreen::OnMouseLeave(EventArgs& e) {
	m_bInClientRect = false;
	MouseLeave(e);
}

void CEScreen::OnMouseFocus(EventArgs& e) {
	MouseFocus(e);
}

void CEScreen::OnMouseBlur(EventArgs& e) {
	MouseBlur(e);
}
