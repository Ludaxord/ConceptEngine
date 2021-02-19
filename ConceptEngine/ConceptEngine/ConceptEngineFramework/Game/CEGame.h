#pragma once
#include <activation.h>
#include <memory>
#include <string>
#include <gainput/gainput.h>

#include "CEConsole.h"
#include "CETimer.h"
#include "CEWindow.h"
#include "../Graphics/CEGraphicsManager.h"

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace ConceptEngineFramework {
	namespace Graphics {
		struct CEGraphicsManager;
	}

	class CEFramework;

	namespace Game {
		class CEConsole;
		class CEWindow;

		class CEGame {
		public:

			virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

			static CEGame& Get();
			uint32_t Run();

			std::shared_ptr<CEConsole> GetConsole();
			void CreateMainWindow(const std::wstring& windowName, int width, int height);
			void CreateConsole(const std::wstring& windowName);
			void CreateGraphicsManager(Graphics::API graphicsAPI);
		protected:
			friend LRESULT CALLBACK ::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst, int width, int height);
			/*
			* Windows message handler
			*/

		private:
			CEGame(std::wstring name, HINSTANCE hInst, int width, int height);

			std::shared_ptr<CEConsole> m_console;
			std::shared_ptr<CEWindow> m_window;
			std::shared_ptr<Graphics::CEGraphicsManager> m_graphicsManager;
			/*
			* Handle to application instance.
			*/
			HINSTANCE m_hInstance;

			CETimer m_timer;
		};
	}
}
