#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Windows {
	class CEWindows final : public Core::Generic::Platform::CEPlatform {
	public:
		CEWindows();
		~CEWindows() override;

		bool Create() override;
		void CreateSystemWindow() override;
		void CreateSystemConsole() override;
		void CreateCursors() override;

		void Update() override;

		static void PreInit(HINSTANCE hInstance,
		                    LPSTR lpCmdLine,
		                    HWND hWnd,
		                    int nCmdShow,
		                    int width,
		                    int height,
		                    bool showConsole);

		static HINSTANCE Instance;
		static HWND HWnd;
		static LPSTR CmdLine;
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
		void GetCursorPosition(Generic::Window::CEWindow* relativeWindow, int32 x, int32 y) override;
	protected:
	private:
	};
}
