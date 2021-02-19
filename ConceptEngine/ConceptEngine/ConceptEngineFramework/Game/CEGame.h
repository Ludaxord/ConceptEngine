#pragma once
#include <activation.h>
#include <memory>
#include <string>
#include <gainput/gainput.h>

#include "CEConsole.h"
#include "CEEvents.h"
#include "CEWindow.h"

#ifdef CreateWindow
#undef CreateWindow
#endif

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace ConceptEngineFramework {
	class CEFramework;

	namespace Game {
		class CEConsole;
		class CEWindow;

		/*
		* Windows message handler
		*/
		using CEWindowProcEvent = Delegate<LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)>;

		class CEGame {
		public:

			static CEGame& Get();
			uint32_t Run();

			std::shared_ptr<CEConsole> GetConsole();
			void CreateWindow(const std::wstring& windowName);
			std::shared_ptr<CEWindow> CreateWindow(const std::wstring& windowName, int width, int height);
			void CreateConsole(const std::wstring& windowName);
			void CreateInputDevices();

		protected:
			friend LRESULT CALLBACK ::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst);
			/*
			* Windows message handler
			*/
			virtual LRESULT OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		private:
			CEGame(std::wstring name, HINSTANCE hInst);


			/*
			 * Invoked when message is sent to window.
			*/
			CEWindowProcEvent WndProcHandler;

			std::shared_ptr<CEConsole> m_console;
			std::shared_ptr<CEWindow> m_window;

			/**
			 * Gainput variables:
			 */
			// gainput::InputManager m_inputManager;
			// gainput::DeviceId m_keyboardDevice;
			// gainput::DeviceId m_mouseDevice;
			// gainput::DeviceId m_gamePadDevice[gainput::MaxPadCount];

			/*
			* Handle to application instance.
			*/
			HINSTANCE m_hInstance;

			/*
			 * Set to true while application is running.
			 */
			std::atomic_bool m_bIsRunning;

			/*
			 * Should application quit?
			 */
			std::atomic_bool m_requestQuit;
		};
	}
}
