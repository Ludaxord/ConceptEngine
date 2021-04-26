#include "CEFramework.h"

#include "Game/CEGame.h"

using namespace ConceptEngineFramework;


CEFramework::CEFramework(std::wstring name,
                         HINSTANCE hInstance,
                         LPSTR lpCmdLine,
                         int nCmdShow,
                         int width,
                         int height,
                         Graphics::API graphicsAPI,
                         Graphics::CEPlayground* playground):
	m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow) {
	Game::CEGame::Create(name, hInstance, width, height, graphicsAPI, playground);
	Game::CEGame::Get().Init();
}

CEFramework::CEFramework(HWND hWnd, Graphics::API graphicsApi, int width, int height,
                         Graphics::CEPlayground* playground): m_hWnd(hWnd) {
	Game::CEGame::Create(hWnd, graphicsApi, width, height, playground);
	Game::CEGame::Get().LinkWithEditor();
}

void CEFramework::EditorRun(int width, int height) const {
	EditorResize(width, height);
	Game::CEGame::Get().GetTimer().Reset();
}

void CEFramework::EditorUpdateTimer() const {
	Game::CEGame::Get().EditorUpdateTimer();
}

void CEFramework::EditorUpdate() const {
	Game::CEGame::Get().EditorUpdate();
}

void CEFramework::EditorRender() const {
	Game::CEGame::Get().EditorRender();
}

void CEFramework::EditorResize(int width, int height) const {
	Game::CEGame::Get().EditorResize(width, height);
}

void CEFramework::EditorKeyDown(WPARAM keyCode, std::string keyChar) {
	Game::CEGame::Get().EditorKeyDown(keyCode, keyChar, Game::CEGame::Get().GetTimer());
}

void CEFramework::EditorMouseMove(WPARAM keyCode, int x, int y) {
	Game::CEGame::Get().EditorMouseMove(keyCode, x, y);
}

void CEFramework::EditorMouseDown(WPARAM keyCode, int x, int y) {
	Game::CEGame::Get().EditorMouseKeyDown(keyCode, x, y);
}

void CEFramework::EditorMouseUp(WPARAM keyCode, int x, int y) {
	Game::CEGame::Get().EditorMouseKeyUp(keyCode, x, y);
}

void CEFramework::EditorMouseWheel(WPARAM keyCode, float wheelDelta, int x, int y) {
	Game::CEGame::Get().EditorMouseWheel(keyCode, wheelDelta, x, y);
}

void CEFramework::EditorChangePlayground(Graphics::CEPlayground* newPlayground) {
	Game::CEGame::Get().ChangePlayground();
}

int CEFramework::Run(bool editorMode) const {
	if (editorMode) {
		return 0;
	}
	return Game::CEGame::Get().Run();
}

HINSTANCE CEFramework::GetHInstance() const {
	return m_hInstance;
}
