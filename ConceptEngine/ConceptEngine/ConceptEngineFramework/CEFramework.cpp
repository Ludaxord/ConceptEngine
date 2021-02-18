#include "CEFramework.h"

#include "Game/CEGame.h"

using namespace ConceptEngineFramework;

CEFramework::CEFramework(std::wstring name, HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow):
	m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow) {
	Game::CEGame::Create(name, hInstance);
}

int CEFramework::Run() {

	return -1;
}

HINSTANCE CEFramework::GetHInstance() {
	return m_hInstance;
}
