#pragma once
#include <activation.h>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Actions/CEEvents.h"
#include "Actions/CEGameTimer.h"

/*
 * Forward declarations
 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace Concept::GameEngine {
	using namespace Actions;
	class CEWindow {
	public:

		static void SetWindowZOrderToTopMost(HWND hWnd, bool setToTopMost) {
			RECT windowRect;
			GetWindowRect(hWnd, &windowRect);

			SetWindowPos(hWnd, (setToTopMost) ? HWND_TOPMOST : HWND_NOTOPMOST, windowRect.left, windowRect.top,
				windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);
		}

		/*
		 * Get handle to OS window instance.
		 *
		 * @returns handle to Os Window instance.
		 */
		HWND GetWindowHandle() const;

		/*
		 * Get current (normalized) DPI scaling for this window
		 *
		 * @returns (normalized) DPI scaling for window
		 */
		float GetDPIScaling() const;

		/*
		 * Get name that was used to create window.
		 *
		 * @returns name that was used to create window.
		 */
		const std::wstring& GetWindowName() const;

		/*
		 * Set window title.
		 *
		 * @param title, new title of window
		 */
		void SetWindowTitle(const std::wstring& windowTitle);

		/*
		 * Get current title of window.
		 *
		 * @returns current title of window
		 */
		const std::wstring& GetWindowTitle() const;

		/*
		 * Get width of window client area
		 *
		 * @returns width of window client area (in pixels)
		 */
		int GetClientWidth() const {
			return m_clientWidth;
		}

		/*
		 * Get height of window client area.
		 *
		 * @returns height of window client area (in pixels)
		 */
		int GetClientHeight() const {
			return m_clientHeight;
		}

		/*
		 * Check to see if window is in fullscreen mode
		 */
		bool IsFullscreen() const;

		/*
		 * Set window to fullscreen state
		 *
		 * @param fullscreen true to set window to fullscreen, false to set windowed mode.
		 */
		void SetFullscreen(bool fullscreen);

		/*
		 * Check to see if cursor is visible
		 */
		bool IsCursorVisible() const;

		/*
		 * Set cursor to visible state
		 *
		 * @param cursorVisible true to set window to make cursor visible, false to set hidden mode.
		 */
		void SetCursor(bool cursorVisible);

		/*
		 * Toggle current fullscreen state of window
		 */
		void ToggleFullscreen();

		/*
		 * Toggle current cursor state of window
		 */
		void ToggleCursor();

		/*
		 * Show window
		 */
		void Show();

		/*
		 * Hide window.
		 */
		void Hide();

		/*
		 * Invoked when game should be updated.
		 */
		UpdateEvent Update;

		/*
		 * DPI scaling of window has changed.
		 */
		DPIScaleEvent DPIScaleChanged;

		/*
		 * Window close event, run when window is about to be closed.
		 */
		WindowCloseEvent Close;

		/*
		 * Invoked when window is resized
		 */
		ResizeEvent Resize;

		/*
		 * Invoked when window is minimized
		 */
		ResizeEvent Minimized;

		/*
		 * Invoked when window is maximized
		 */
		ResizeEvent Maximized;

		/*
		 * Invoked when window is restored
		 */
		ResizeEvent Restored;

		/*
		 * Invoked when keyboard key is pressed while window has focus
		 */
		KeyboardEvent KeyPressed;

		/*
		 * Invoked when keyboard key is released while window has focus.
		 */
		KeyboardEvent KeyReleased;

		/*
		 * Invoked when window gains keyboard focus
		 */
		Event KeyboardFocus;

		/*
		 * Invoked when window loses keyboard focus
		 */
		Event KeyboardBlur;

		/*
		 * Invoked when mouse is moved over the window.
		 */
		MouseMotionEvent MouseMoved;

		/*
		 * Invoked when mouse enters client area
		 */
		MouseMotionEvent MouseEnter;

		/*
		 * Invoke when mouse button is pressed over window
		 */
		MouseButtonEvent MouseButtonPressed;

		/*
		 * Invoked when mouse button is released over window
		 */
		MouseButtonEvent MouseButtonReleased;

		/*
		 * Invoked when mouse wheel is scrolled over window
		 */
		MouseWheelEvent MouseWheel;

		/*
		 * Invoked when mouse cursor leaves client area.
		 */
		Event MouseLeave;

		/*
		 * Invoked when window gains mouse focus.
		 */
		Event MouseFocus;

		/*
		 * Invoked when window looses mouse focus
		 */
		Event MouseBlur;
	protected:
		friend class CEGame;
		/*
		 * It is required to allow WndProc function to call event callbacks on window
		 */
		friend LRESULT CALLBACK::WndProc(HWND, UINT, WPARAM, LPARAM);

		/*
		 * Only Application can create windows.
		 */
		CEWindow(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight);

		virtual ~CEWindow();

		/*
		 * Update game
		 */
		virtual void OnUpdate(UpdateEventArgs& e);

		/*
		 * DPI scaling of window has changed
		 */
		virtual void OnDPIScaleChanged(DPIScaleEventArgs& e);

		/*
		 * Window was closed
		 */
		virtual void OnClose(WindowCloseEventArgs& e);

		/*
		 * Window was resized
		 */
		virtual void OnResize(ResizeEventArgs& e);

		/*
		 * Window was minimized
		 */
		virtual void OnMinimized(ResizeEventArgs& e);

		/*
		 * Window was maximized
		 */
		virtual void OnMaximized(ResizeEventArgs& e);

		/*
		 * Window was restored
		 */
		virtual void OnRestored(ResizeEventArgs& e);

		/*
		 * Keyboard key was pressed
		 */
		virtual void OnKeyPressed(KeyEventArgs& e);

		/*
		 * Keyboard key was released
		 */
		virtual void OnKeyReleased(KeyEventArgs& e);

		/*
		 * Window gained keyboard focus
		 */
		virtual void OnKeyboardFocus(EventArgs& e);

		/*
		 * Window lost keyboard focus
		 */
		virtual void OnKeyboardBlur(EventArgs& e);

		/*
		 * Mouse was moved
		 */
		virtual void OnMouseMoved(MouseMotionEventArgs& e);

		/*
		 * Mouse button was pressed
		 */
		virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);

		/*
		 * Mouse button was released
		 */
		virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);

		/*
		 * Mouse wheel was moved
		 */
		virtual void OnMouseWheel(MouseWheelEventArgs& e);

		/*
		 * Mouse entered client area
		 */
		virtual void OnMouseEnter(MouseMotionEventArgs& e);

		/*
		 * Mouse left client area of window
		 */
		virtual void OnMouseLeave(EventArgs& e);

		/*
		 * Window gain mouse focus
		 */
		virtual void OnMouseFocus(EventArgs& e);

		/*
		 * Window lost mouse focus
		 */
		virtual void OnMouseBlur(Actions::EventArgs& e);

	private:
		HWND m_hWnd;

		std::wstring m_name;
		std::wstring m_title;

		uint32_t m_clientWidth;
		uint32_t m_clientHeight;

		int32_t m_previousMouseX;
		int32_t m_previousMouseY;

		/*
		 * Get current dpi scaling of window
		 */
		float m_DPIScaling;

		/*
		 * Current fullscreen state of window
		 */
		bool m_isFullscreen;

		/*
		 * Current cursor state of window
		 */
		bool m_cursorVisible;

		/*
		 * Check if window is minimized
		 */
		bool m_isMinimized;

		/*
		 * Check if window is maximized
		 */
		bool m_isMaximized;

		/*
		 * Check if mouse is inside window client rect
		 */
		bool m_bInClientRect;

		RECT m_windowRect;

		/*
		 * Check if window receives keyboard focus
		 */
		bool m_bHasKeyboardFocus;
		CEGameTimer m_timer;
	};
}
