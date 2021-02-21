#pragma once
#include <activation.h>
#include <memory>
#include <string>
#include <gainput/gainput.h>

#include "CEConsole.h"
#include "CETimer.h"
#include "CEWindow.h"
#include "../Graphics/CEGraphicsManager.h"
#include "../Tools/CEUtils.h"

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

			virtual void OnMouseDown(WPARAM btnState, int x, int y) {
			}

			virtual void OnMouseUp(WPARAM btnState, int x, int y) {
			}

			virtual void OnMouseMove(WPARAM btnState, int x, int y) {
			}

			virtual void OnKeyUp(WPARAM btnState) {
			}

			virtual void OnKeyDown(WPARAM btnState) {
			}

		protected:
			friend LRESULT CALLBACK ::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst, int width, int height, Graphics::API graphicsAPI);

			void Init();
			void SystemInfo();
			void CalculateFPS(bool showInTitleBar = false) const;

		private:
			CEGame(std::wstring name, HINSTANCE hInst, int width, int height, Graphics::API graphicsAPI);

			std::shared_ptr<CEConsole> m_console;
			std::shared_ptr<CEWindow> m_window;
			std::shared_ptr<Graphics::CEGraphicsManager> m_graphicsManager;
			/*
			* Handle to application instance.
			*/
			HINSTANCE m_hInstance;

			CETimer m_timer;

			Graphics::API m_graphicsAPI;

			CESystemInfo m_systemInfo;
			std::wstring m_name;
			int m_width;
			int m_height;

			bool m_paused = false; // is the application paused?
			bool m_minimized = false; // is the application minimized?
			bool m_maximized = false; // is the application maximized?
			bool m_resizing = false; // are the resize bars being dragged?
		};
	}
}
