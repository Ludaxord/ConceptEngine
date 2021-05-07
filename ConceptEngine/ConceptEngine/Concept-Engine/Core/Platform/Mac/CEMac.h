#pragma once
#include "../Generic/CEPlatform.h"

namespace ConceptEngine::Core::Platform::Mac {
	class CEMac final : public Core::Generic::Platform::CEPlatform {
	public:
		CEMac();
		~CEMac() override;

		bool Create() override;
		void CreateSystemWindow() override;
		void CreateSystemConsole() override;
		void CreateCursors() override;

		void Update() override;

		static void PreInit();
	private:
		bool CreateInputManager() override;
	public:
		bool Release() override;
		Generic::Input::CEModifierKeyState GetModifierKeyState() override;
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
