#pragma once

#include "../../Generic/Window/CEWindow.h"

namespace ConceptEngine::Core::Platform::Windows::Window {
	class CEWindowsWindowSize : public Generic::Window::CEWindowSize {
	public:
		static void Create(int width, int height) {
			WndSize = new CEWindowsWindowSize(width, height);
			WndSize->CreateSize();

		}

		void CreateSize(int width, int height) override {
			GetScreenResolution(ScreenWidth, ScreenHeight);
		};

	private:
		CEWindowsWindowSize(int width, int height): CEWindowSize(width, height) {

		}
	};

	class CEWindowsWindow final : public Generic::Window::CEWindow {
	public:
		CEWindowsWindow();
		~CEWindowsWindow() override;

		bool Create() override;
	protected:
	private:
	};
}
