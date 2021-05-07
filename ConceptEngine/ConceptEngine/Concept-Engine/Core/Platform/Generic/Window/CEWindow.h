#pragma once
#include "CEScreen.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	class CEWindowSize {
	public:
		virtual ~CEWindowSize() = default;

		static void SetResolution(int width, int height) {
			SetWidth(width);
			SetHeight(height);
		}

		static void SetWidth(int width) {
			Width = width;
		}

		static int GetWidth() {
			if (Width == 0) {
				Width = ScreenWidth;
			}
			return Width;
		}

		static void SetHeight(int height) {
			Height = height;
		}

		static int GetHeight() {
			if (Height == 0) {
				Height = ScreenHeight;
			}
			return Height;
		}

		virtual void CreateSize(int width = 0, int height = 0) = 0;

	protected:
		CEWindowSize(int width, int height) {
			SetResolution(width, height);
		}

		static CEWindowSize* WndSize;

		static int Width;
		static int Height;

		static int ScreenWidth;
		static int ScreenHeight;
	};

	class CEWindow : public CEScreen {
	public:
		CEWindow();
		~CEWindow();
	};
}
