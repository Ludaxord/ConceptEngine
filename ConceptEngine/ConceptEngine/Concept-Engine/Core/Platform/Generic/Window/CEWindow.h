#pragma once
#include <string>

#include "../../../Common/CETypes.h"

#include "CEScreen.h"

namespace ConceptEngine {namespace Core {namespace Generic {namespace Platform {
	class CEPlatform;
}}}}

namespace ConceptEngine::Core::Platform::Generic { namespace Window {
	enum CEEnumWindowStyleFlag : uint32 {
		WindowStyleFlag_None = 0x0,
		WindowStyleFlag_Titled = FLAG(1),
		WindowStyleFlag_Closable = FLAG(2),
		WindowStyleFlag_Minimizable = FLAG(3),
		WindowStyleFlag_Maximizable = FLAG(4),
		WindowStyleFlag_Resizable = FLAG(5),
	};

	struct CEWindowPosition {
		int x = 0;
		int y = 0;
	};

	struct CEWindowStyle {
		CEWindowStyle() = default;

		CEWindowStyle(uint32 style) : Style(style) {

		}

		bool IsTitled() const {
			return Style & WindowStyleFlag_Titled;
		}

		bool IsClosable() const {
			return Style & WindowStyleFlag_Closable;
		}

		bool IsMinimizable() const {
			return Style & WindowStyleFlag_Minimizable;
		}

		bool IsMaximizable() const {
			return Style & WindowStyleFlag_Maximizable;
		}

		bool IsResizable() const {
			return Style & WindowStyleFlag_Resizable;
		}

		uint32 Style = 0;
	};

	class CEWindowSize {
	public:
		virtual ~CEWindowSize() = default;

		static void SetResolution(int width, int height) {
			SetWidth(width);
			SetHeight(height);
		}

		static void SetPosition(int x, int y) {
			WindowPosition = {x, y};
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
		CEWindowSize(int width, int height, int x, int y) {
			SetResolution(width, height);
			SetPosition(x, y);
		}

	public:
		static int Width;
		static int Height;

		static int ScreenWidth;
		static int ScreenHeight;

		static CEWindowPosition WindowPosition;

	protected:
		friend class Core::Generic::Platform::CEPlatform;
		static CEWindowSize* WndSize;
	};

	class CEWindow : public CEScreen {
	public:
		CEWindow();
		~CEWindow();

		virtual bool Create(const std::string& title, uint32 width, uint32 height, CEWindowStyle style) = 0;

		virtual bool RegisterWindowClass() = 0;

		virtual void Show(bool maximized) = 0;
		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Close() = 0;
		virtual void Restore() = 0;
		virtual void ToggleFullscreen() = 0;

		virtual bool IsValid() const = 0;
		virtual bool IsActiveWindow() const = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void GetTitle(std::string& outTitle) = 0;

		virtual void SetWindowSize(const CEWindowSize& shape, bool move) = 0;
		virtual void GetWindowSize(CEWindowSize& outShape) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		virtual void* GetNativeHandle() const {
			return nullptr;
		}

		CEWindowStyle GetStyle() const {
			return WindowStyle;
		}

	protected:
		CEWindowStyle WindowStyle;
	};
}}
