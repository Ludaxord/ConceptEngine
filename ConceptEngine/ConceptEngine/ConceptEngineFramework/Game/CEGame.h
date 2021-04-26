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

namespace ConceptEngineFramework { namespace Graphics {
		class CEPlayground;
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
			void CreateEditorWindow(const std::wstring& windowName, HWND hwnd, int width, int height);
			void CreateConsole(const std::wstring& windowName);
			void CreateGraphicsManager(Graphics::API graphicsAPI);
			void CreateEditorGraphicsManager(Graphics::API graphicsAPI);

			virtual void OnMouseDown(WPARAM btnState, int x, int y) {
				const auto keyState = WParamToKeyCode(btnState);
				m_playground->OnMouseDown(keyState, x, y);
			}

			virtual void OnMouseUp(WPARAM btnState, int x, int y) {
				const auto keyState = WParamToKeyCode(btnState);
				m_playground->OnMouseUp(keyState, x, y);
			}

			virtual void OnMouseMove(WPARAM btnState, int x, int y) {
				const auto keyState = WParamToKeyCode(btnState);
				m_playground->OnMouseMove(keyState, x, y);
			}

			virtual void OnKeyUp(WPARAM btnState, LPARAM btnValue, const CETimer& gt) {
				auto keyState = WParamToKeyCode(btnState);
				auto keyChar = CharFromParams(btnState, btnValue);
				m_playground->OnKeyUp(keyState, (char)keyChar, gt);
			}

			virtual void OnKeyDown(WPARAM btnState, LPARAM btnValue, const CETimer& gt) {
				spdlog::info(btnState);
				auto keyState = WParamToKeyCode(btnState);
				auto keyChar = CharFromParams(btnState, btnValue);
				m_playground->OnKeyDown(keyState, (char)keyChar, gt);
			}

			virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam, HWND hwnd) {
				auto keyState = WParamToKeyCode(wParam);
				float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
				int x = ((int)(short)LOWORD(lParam));
				int y = ((int)(short)HIWORD(lParam));
				POINT screenToClientPoint;
				screenToClientPoint.x = x;
				screenToClientPoint.y = y;
				::ScreenToClient(hwnd, &screenToClientPoint);
				m_playground->OnMouseWheel(keyState, zDelta, (int)screenToClientPoint.x, (int)screenToClientPoint.y);
			}

			//FOR EDITOR
			CETimer GetTimer() const;
			void EditorUpdateTimer();
			void EditorUpdate() const;
			void EditorRender() const;
			void EditorResize(int width, int height) const;

			virtual void EditorKeyDown(WPARAM keyCode, std::string keyChar, const CETimer& gt) {
				auto keyState = WParamToKeyCode(keyCode);
				m_playground->OnKeyDown(keyState, reinterpret_cast<char>(keyChar.c_str()), gt);
			}

		protected:
			friend LRESULT CALLBACK ::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			friend CEFramework;
			static CEGame& Create(std::wstring name, HINSTANCE hInst, int width, int height, Graphics::API graphicsAPI,
			                      Graphics::CEPlayground* playground);
			static CEGame& Create(HWND hWnd, Graphics::API graphicsAPI, int width, int height,
			                      Graphics::CEPlayground* playground);

			void Init();
			void LinkWithEditor();
			void SystemInfo();
			void CalculateFPS(bool showInTitleBar = false) const;

		private:
			CEGame(std::wstring name, HINSTANCE hInst, int width, int height, Graphics::API graphicsAPI,
			       Graphics::CEPlayground* playground);
			CEGame(HWND hWnd, Graphics::API graphicsAPI, int width, int height, Graphics::CEPlayground* playground);

			std::shared_ptr<CEConsole> m_console;
			std::shared_ptr<CEWindow> m_window;
			std::shared_ptr<Graphics::CEGraphicsManager> m_graphicsManager;
			Graphics::CEPlayground* m_playground;
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

			HWND m_hwnd;
		};
	}}
