#include "CEGame.h"

#include <spdlog/spdlog.h>
#include <windowsx.h>

#include "CEConsole.h"
#include "../Graphics/DirectX12/CEDX12Manager.h"
#include "../Graphics/Vulkan/CEVManager.h"
#include "../Graphics/OpenGL/CEOGLManager.h"

namespace GameEngine = ConceptEngineFramework::Game;
namespace GraphicsEngine = ConceptEngineFramework::Graphics;
namespace DirectXGraphicsEngine = GraphicsEngine::DirectX12;
namespace VulkanGraphicsEngine = GraphicsEngine::Vulkan;
namespace OpenGLGraphicsEngine = GraphicsEngine::OpenGL;

static GameEngine::CEGame* g_pGame = nullptr;

/*
 * Instance for std::shared_ptr
 */
class CEConsoleInstance final : public GameEngine::CEConsole {

public:
	CEConsoleInstance(const std::wstring& windowName, int maxFileSizeInMB = 5, int maxFiles = 3,
	                  int maxConsoleLines = 500)
		: CEConsole(windowName, maxFileSizeInMB, maxFiles, maxConsoleLines) {
		spdlog::info("ConceptEngineFramework Console class created.");
	}

	explicit CEConsoleInstance(const Logger& logger)
		: CEConsole(logger) {
		spdlog::info("ConceptEngineFramework Console class created.");
	}
};

class CEWindowInstance final : public GameEngine::CEWindow {

public:
	CEWindowInstance(const std::wstring& windowName, HINSTANCE hInstance, int width, int height)
		: CEWindow(windowName, hInstance, width, height) {
		spdlog::info("ConceptEngineFramework Window class created.");
	}

	CEWindowInstance(const std::wstring& windowName, HWND hwnd, int width, int height)
		: CEWindow(windowName, hwnd, width, height) {
		spdlog::info("ConceptEngineFramework Window class created.");
	}
};

class CEDX12ManagerInstance final : public DirectXGraphicsEngine::CEDX12Manager {
public:
	CEDX12ManagerInstance(ConceptEngineFramework::Game::CEWindow* window): CEDX12Manager(window) {
		spdlog::info("ConceptEngineFramework DirectX 12 Manager class created.");
	}

	CEDX12ManagerInstance(): CEDX12Manager() {
		spdlog::info("ConceptEngineFramework DirectX 12 Manager class created.");
	}
};

class CEVManagerInstance final : public VulkanGraphicsEngine::CEVManager {
public:
	CEVManagerInstance(): CEVManager() {
		spdlog::info("ConceptEngineFramework Vulkan Manager class created.");
	}
};

class CEOGLManagerInstance final : public OpenGLGraphicsEngine::CEOGLManager {
public:
	CEOGLManagerInstance(): CEOGLManager() {
		spdlog::info("ConceptEngineFramework OpenGL Manager class created.");
	}
};


GameEngine::CEGame::CEGame(std::wstring name,
                           HINSTANCE hInst,
                           int width,
                           int height,
                           Graphics::API graphicsAPI,
                           Graphics::CEPlayground* playground) :
	m_name(name),
	m_hInstance(hInst),
	m_width(width),
	m_height(height),
	m_graphicsAPI(graphicsAPI),
	m_systemInfo{},
	m_playground(playground) {
}

GameEngine::CEGame::CEGame(HWND hWnd, Graphics::API graphicsAPI, int width, int height,
                           Graphics::CEPlayground* playground): m_hwnd(hWnd),
                                                                m_graphicsAPI(graphicsAPI),
                                                                m_systemInfo{},
                                                                m_width(width),
                                                                m_height(height),
                                                                m_playground(playground) {
}

void GameEngine::CEGame::Init() {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	CreateConsole(m_name);
	CreateMainWindow(m_name, m_width, m_height);
	m_window->InitWindow();
	SystemInfo();
	CreateGraphicsManager(m_graphicsAPI);
}

void GameEngine::CEGame::LinkWithEditor() {
	CreateEditorWindow(m_name, m_hwnd, m_width, m_height);
	SystemInfo();
	CreateEditorGraphicsManager(m_graphicsAPI);
}

GameEngine::CEGame& GameEngine::CEGame::Create(std::wstring name, HINSTANCE hInst, int width, int height,
                                               Graphics::API graphicsAPI, Graphics::CEPlayground* playground) {
	if (!g_pGame) {
		g_pGame = new CEGame(name, hInst, width, height, graphicsAPI, playground);
		spdlog::info("ConceptEngineFramework Game class created.");
	}

	return *g_pGame;
}

ConceptEngineFramework::Game::CEGame& ConceptEngineFramework::Game::CEGame::Create(
	HWND hWnd, Graphics::API graphicsAPI, int width, int height,
	Graphics::CEPlayground* playground) {
	if (!g_pGame) {
		g_pGame = new CEGame(hWnd, graphicsAPI, width, height, playground);
		spdlog::info("ConceptEngineFramework Game class created.");
	}

	return *g_pGame;
}

void ConceptEngineFramework::Game::CEGame::SystemInfo() {
	m_systemInfo = GetEngineSystemInfo();
	spdlog::info("CPU: {}, Threads: {}, RAM: {} MB", m_systemInfo.CPUName, m_systemInfo.CPUCores, m_systemInfo.RamSize);
}

void ConceptEngineFramework::Game::CEGame::CalculateFPS(bool showInTitleBar) const {
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	//Compute averages over one second period.
	if ((m_timer.TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCount;
		float msPerFrame = 1000.0f / fps;

		spdlog::info("FPS: {}", fps);
		spdlog::info("MSPF: {}", msPerFrame);
		if (showInTitleBar) {
			std::wstring wFps = std::to_wstring(fps);
			std::wstring wMsPerFrame = std::to_wstring(msPerFrame);
			std::wstring fpsWindowName = m_window->GetName() +
				L"    fps: " + wFps +
				L"   mspf: " + wMsPerFrame;
		}

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT GameEngine::CEGame::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		/*
		 * NOTE: Potential reason of bug 
		if (LOWORD(wParam) == WA_INACTIVE) {
			m_paused = true;
			m_timer.Stop();
		}
		else {
			m_paused = false;
			m_timer.Start();
		}
		*/
		return 0;
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_window->SetResolution(LOWORD(lParam), HIWORD(lParam));
		if (m_graphicsManager) {
			if (m_graphicsManager->Initialized()) {
				if (wParam == SIZE_MINIMIZED) {
					m_paused = true;
					m_minimized = true;
					m_maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED) {
					m_paused = false;
					m_minimized = false;
					m_maximized = true;
					m_graphicsManager->Resize();
				}
				else if (wParam == SIZE_RESTORED) {
					if (m_minimized) {
						m_paused = false;
						m_minimized = false;
						m_graphicsManager->Resize();
					}
					else if (m_maximized) {
						m_paused = false;
						m_maximized = false;
						m_graphicsManager->Resize();
					}
					else if (m_resizing) {
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else {
						m_graphicsManager->Resize();
					}
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		m_paused = true;
		m_resizing = true;
		m_timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		m_paused = false;
		m_resizing = false;
		m_timer.Start();
		m_graphicsManager->Resize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		else {
			OnKeyUp(wParam, lParam, m_timer);
		}
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		OnKeyDown(wParam, lParam, m_timer);
		OnKeyDown(wParam, lParam, m_timer);
		return 0;
	case WM_MOUSEWHEEL: {
		OnMouseWheel(wParam, lParam, hwnd);
	}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

GameEngine::CEGame& GameEngine::CEGame::Get() {
	assert(g_pGame != nullptr);
	return *g_pGame;
}

uint32_t GameEngine::CEGame::Run() {

	MSG msg = {0};

	m_timer.Reset();

	while (msg.message != WM_QUIT) {
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
			// Otherwise, do animation/game stuff.
		else {
			m_timer.Tick();

			if (!m_paused) {
				CalculateFPS(true);
				m_graphicsManager->Update(m_timer);
				m_graphicsManager->Render(m_timer);
			}
			else {
				Sleep(100);
			}

		}
	}

	return (int)msg.wParam;
}


std::shared_ptr<GameEngine::CEConsole> GameEngine::CEGame::GetConsole() {
	return m_console;
}

void GameEngine::CEGame::CreateMainWindow(const std::wstring& windowName, int width, int height) {
	m_window = std::make_shared<CEWindowInstance>(windowName, m_hInstance, width, height);
	m_window->Create();
}

void ConceptEngineFramework::Game::CEGame::CreateEditorWindow(const std::wstring& windowName, HWND hwnd, int width,
                                                              int height) {
	m_window = std::make_shared<CEWindowInstance>(windowName, hwnd, width, height);
}

void GameEngine::CEGame::CreateGraphicsManager(Graphics::API graphicsAPI) {
	switch (graphicsAPI) {
	case Graphics::API::DirectX12_API:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>(m_window.get());
		break;
	case Graphics::API::Vulkan_API:
		m_graphicsManager = std::make_shared<CEVManagerInstance>();
		break;
	case Graphics::API::OpenGL_API:
		m_graphicsManager = std::make_shared<CEOGLManagerInstance>();
		break;
	default:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>(m_window.get());
		break;
	}
	m_graphicsManager->InitPlayground(m_playground);
	m_graphicsManager->Create();

}

void ConceptEngineFramework::Game::CEGame::CreateEditorGraphicsManager(Graphics::API graphicsAPI) {
	switch (graphicsAPI) {
	case Graphics::API::DirectX12_API:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>(m_window.get());
		break;
	case Graphics::API::Vulkan_API:
		m_graphicsManager = std::make_shared<CEVManagerInstance>();
		break;
	case Graphics::API::OpenGL_API:
		m_graphicsManager = std::make_shared<CEOGLManagerInstance>();
		break;
	default:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>(m_window.get());
		break;
	}
	m_graphicsManager->InitPlayground(m_playground);
	m_graphicsManager->Create();
}

void GameEngine::CEGame::CreateConsole(const std::wstring& windowName) {
	m_console = std::make_shared<CEConsoleInstance>(windowName);
	m_console->Create();
}
