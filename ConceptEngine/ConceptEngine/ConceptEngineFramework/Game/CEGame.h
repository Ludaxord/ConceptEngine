#pragma once
#include <activation.h>
#include <memory>
#include <string>
#include <gainput/gainput.h>

#include "CEConsole.h"
#include "CEWindow.h"

namespace ConceptEngineFramework {
	class CEFramework;

	namespace Game {
		class CEConsole;

		class CEGame {
		public:

			static CEGame& Get();
			uint32_t Run();
			
			std::shared_ptr<CEConsole> GetConsole();
			std::shared_ptr<CEWindow> CreateWindow(const std::wstring& windowName, int clientWidth, int clientHeight);
		protected:
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst);

		private:
			CEGame(std::wstring name, HINSTANCE hInst);
			std::shared_ptr<CEConsole> m_console;


			/**
			 * Gainput variables:
			 */
			// gainput::InputManager m_inputManager;
			// gainput::DeviceId m_keyboardDevice;
			// gainput::DeviceId m_mouseDevice;
			// gainput::DeviceId m_gamePadDevice[gainput::MaxPadCount];

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
