#pragma once
#include "../Generic/CEPlatform.h"
#include <Windows.h>

namespace ConceptEngine::Core::Platform::Windows {namespace Window {
		class CEWindowsWindow;
	}

	class CEWindows final : public Core::Generic::Platform::CEPlatform {
	public:
		CEWindows();
		~CEWindows() override;

		bool Create() override;
		bool CreateSystemWindow() override;
		bool CreateSystemConsole() override;
		bool CreateCursors() override;

		void Update() override;

		static void PreInit(HINSTANCE hInstance,
		                    LPSTR lpCmdLine,
		                    HWND hWnd,
		                    int nCmdShow,
		                    int width,
		                    int height,
		                    bool showConsole);

		static LPCWSTR GetWindowClassName() {
			return L"WindowClass";
		}

		static HINSTANCE Instance;
		static HWND HWnd;
		static int CmdShow;

	private:
		bool CreateInputManager() override;

	public:
		bool Release() override;
		void SetCapture(Generic::Window::CEWindow* window) override;
		void SetActiveWindow(Generic::Window::CEWindow* window) override;
		Generic::Window::CEWindow* GetCapture() override;
		Generic::Window::CEWindow* GetActiveWindow() override;
		void SetCursor(Generic::Cursor::CECursor* cursor) override;
		Generic::Cursor::CECursor* GetCursor() override;
		void SetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) override;
		void GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32& x, int32& y) override;

		void UpdateDefaultGame();

		void UpdatePeekMessage();
		void UpdateStoredMessage();

		Generic::Input::CEModifierKeyState GetModifierKeyState() override;
	protected:
	private:
		friend class Window::CEWindowsWindow;
		static LRESULT MessageProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
		static void HandleStoredMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
		static void StoreMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
