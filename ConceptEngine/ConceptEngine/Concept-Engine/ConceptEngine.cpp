#include "ConceptEngine.h"

#include "Core/Application/Game/CEGame.h"
#include "Core/Application/Editor/CEEditor.h"
#include "Core/Application/GameDebug/CEGameDebug.h"
#include "Utilities/CEScreenUtilities.h"

ConceptEngine::ConceptEngine::ConceptEngine(std::wstring name,
                                            HINSTANCE hInstance,
                                            LPSTR lpCmdLine,
                                            int nCmdShow,
                                            int width,
                                            int height, bool showConsole,
                                            Graphics::Main::GraphicsAPI api,
                                            Core::Compilers::Language language): m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow),
	m_api(api),
	m_language(language) {
	if (showConsole) {
		Create(EngineBoot::GameDebug);
	}
	else {
		Create(EngineBoot::Game);
	}
}

ConceptEngine::ConceptEngine::ConceptEngine(std::wstring name,
                                            HINSTANCE hInstance,
                                            LPSTR lpCmdLine,
                                            int nCmdShow,
                                            bool showConsole,
                                            Graphics::Main::GraphicsAPI api,
                                            Core::Compilers::Language language) : m_name(name),
	m_hInstance(hInstance),
	m_lpCmdLine(lpCmdLine),
	m_nCmdShow(nCmdShow),
	m_api(api),
	m_language(language) {
	int width, height;
	GetScreenResolution(width, height);
	if (showConsole) {
		Create(EngineBoot::GameDebug);
	}
	else {
		Create(EngineBoot::Game);
	}
}

ConceptEngine::ConceptEngine::ConceptEngine(std::wstring name,
                                            HWND hWnd,
                                            int width,
                                            int height,
                                            Graphics::Main::GraphicsAPI api) : m_name(name),
                                                                               m_hwnd(hWnd),
                                                                               m_api(api) {
	Create(EngineBoot::Editor);
}

int ConceptEngine::ConceptEngine::Run() const {
	return m_core->Run();
}

bool ConceptEngine::ConceptEngine::Create(EngineBoot boot) {
	switch (boot) {
	case EngineBoot::Game:
		return CreateGame();
	case EngineBoot::Editor:
		return CreateEditor();
	case EngineBoot::GameDebug:
		return CreateGameDebug();
	}
	return false;
}

ConceptEngine::Core::Application::CECore* ConceptEngine::ConceptEngine::GetCore() const {
	return m_core.get();
}

bool ConceptEngine::ConceptEngine::CreateEditor() {
	auto editor = std::make_unique<Core::Application::Editor::CEEditor>(m_api, m_language);
	m_core = std::move(editor);
	return true;
}

bool ConceptEngine::ConceptEngine::CreateGame() {
	auto game = std::make_unique<Core::Application::Game::CEGame>(m_api, m_language);
	m_core = std::move(game);
	return true;
}

bool ConceptEngine::ConceptEngine::CreateGameDebug() {
	auto gameDebug = std::make_unique<Core::Application::GameDebug::CEGameDebug>(m_api, m_language);
	m_core = std::move(gameDebug);
	return true;
}
