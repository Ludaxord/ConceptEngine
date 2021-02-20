#include "CEFramework.h"

#include "Game/CEGame.h"

using namespace ConceptEngineFramework;


CEFramework::CEFramework(std::wstring name,
                         HINSTANCE hInstance,
                         LPSTR lpCmdLine,
                         int nCmdShow,
                         int width,
                         int height,
                         Graphics::API graphicsAPI):
	m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow) {
	Game::CEGame::Create(name, hInstance, width, height, graphicsAPI);
	Game::CEGame::Get().Init();
}

int CEFramework::Run() const {
	return Game::CEGame::Get().Run();
}

HINSTANCE CEFramework::GetHInstance() {
	return m_hInstance;
}
