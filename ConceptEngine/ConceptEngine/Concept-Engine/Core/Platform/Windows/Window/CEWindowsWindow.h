#pragma once

#include "../../Generic/Window/CEWindow.h"

namespace ConceptEngine::Core::Platform::Windows::Window {
	static const uint32 DefaultStyle =
		Generic::Window::WindowStyleFlag_Titled |
		Generic::Window::WindowStyleFlag_Closable |
		Generic::Window::WindowStyleFlag_Minimizable |
		Generic::Window::WindowStyleFlag_Maximizable |
		Generic::Window::WindowStyleFlag_Resizable;

	class CEWindowsWindow;

	class CEWindowsWindowSize : public Generic::Window::CEWindowSize {
	public:
		static void Create(int width, int height, int x = 0, int y = 0) {
			Generic::Window::WndSize = new CEWindowsWindowSize(width, height, x, y);
			Generic::Window::WndSize->CreateSize();

		}

		void CreateSize(int width, int height) override {
			GetScreenResolution(ScreenWidth, ScreenHeight);
		};

		friend class CEWindowsWindow;
		CEWindowsWindowSize(int width, int height, int x, int y): CEWindowSize(width, height, x, y) {

		}
	};

	class CEWindowsWindow final : public Generic::Window::CEWindow {
	public:
		CEWindowsWindow();
		~CEWindowsWindow() override;

		bool Create() override;
		bool RegisterWindowClass() override;

		bool Create(const std::string& title, uint32 width, uint32 height,
		            Generic::Window::CEWindowStyle style) override;
		void Show(bool maximized) override;
		void Minimize() override;
		void Maximize() override;
		void Close() override;
		void Restore() override;
		void ToggleFullscreen() override;
		bool IsValid() const override;
		bool IsActiveWindow() const override;
		void SetTitle(const std::string& title) override;
		void GetTitle(std::string& outTitle) override;
		void SetWindowSize(const Generic::Window::CEWindowSize& shape, bool move) override;
		void GetWindowSize(Generic::Window::CEWindowSize& outShape) override;
		uint32 GetWidth() const override;
		uint32 GetHeight() const override;
		HWND GetHandle() const;
	private:
	protected:
	private:
		DWORD Style;
		DWORD StyleEx;

		bool IsFullscreen;
		WINDOWPLACEMENT StoredPlacement;
	};

	struct CEWindowsWindowHandle {
		CEWindowsWindowHandle() = default;

		CEWindowsWindowHandle(HWND handle) : Handle(handle) {

		}

		CEWindowsWindow* GetWindow() const {
			CEWindowsWindow* window = (CEWindowsWindow*)GetWindowLongPtrA(Handle, GWLP_USERDATA);
			if (window) {
				window->AddRef();
			}
			return window;
		}

		HWND Handle;
	};
}
