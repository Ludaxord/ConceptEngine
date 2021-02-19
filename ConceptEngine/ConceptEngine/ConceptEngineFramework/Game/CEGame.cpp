#include "CEGame.h"

#include <spdlog/spdlog.h>


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
};

class CEDX12ManagerInstance final : public DirectXGraphicsEngine::CEDX12Manager {
public:
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


GameEngine::CEGame::CEGame(std::wstring name, HINSTANCE hInst, int width, int height) : m_hInstance(hInst) {
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	CreateConsole(name);
	CreateMainWindow(name, width, height);
}

GameEngine::CEGame& GameEngine::CEGame::Create(std::wstring name, HINSTANCE hInst, int width, int height) {
	if (!g_pGame) {
		g_pGame = new CEGame(name, hInst, width, height);
		spdlog::info("ConceptEngineFramework Game class created.");
	}

	return *g_pGame;
}

LRESULT GameEngine::CEGame::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_window->SetResolution(LOWORD(lParam), HIWORD(lParam));
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:

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
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		return 0;
	case WM_MOUSEMOVE:
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2) {
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
	m_window->InitWindow();

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

void GameEngine::CEGame::CreateGraphicsManager(Graphics::API graphicsAPI) {
	switch (graphicsAPI) {
	case Graphics::API::DirectX12_API:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>();
		break;
	case Graphics::API::Vulkan_API:
		m_graphicsManager = std::make_shared<CEVManagerInstance>();
		break;
	case Graphics::API::OpenGL_API:
		m_graphicsManager = std::make_shared<CEOGLManagerInstance>();
		break;
	default:
		m_graphicsManager = std::make_shared<CEDX12ManagerInstance>();
		break;
	}
	m_graphicsManager->Create();
}

void GameEngine::CEGame::CreateConsole(const std::wstring& windowName) {
	m_console = std::make_shared<CEConsoleInstance>(windowName);
	m_console->Create();
}
