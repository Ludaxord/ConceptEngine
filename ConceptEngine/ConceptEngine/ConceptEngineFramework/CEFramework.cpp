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

void CEFramework::EditorUpdate() {
	Game::CEGame::Get().EditorUpdate();
}

void CEFramework::EditorRender() {
	Game::CEGame::Get().EditorRender();
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
