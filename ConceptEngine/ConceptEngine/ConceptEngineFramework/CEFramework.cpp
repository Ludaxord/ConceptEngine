#include "CEFramework.h"

#include "Game/CEGame.h"
#include "Game/Playgrounds/CEDXInitPlayground.h"
#include "Game/Playgrounds/CEPlayground.h"

using namespace ConceptEngineFramework;

CEFramework::CEFramework(std::wstring name, HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow):
	m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow) {
	Game::CEGame::Create(name, hInstance);
}

int CEFramework::Run() {
	std::unique_ptr<Game::Playgrounds::CEPlayground> playground = std::make_unique<Game::Playgrounds::CEDXInitPlayground
	>(L"Concept Engine DirectX Init", 1920, 1080);
	return playground->Run();
}

HINSTANCE CEFramework::GetHInstance() {
	return m_hInstance;
}
