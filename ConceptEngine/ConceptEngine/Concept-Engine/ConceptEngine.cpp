#include "ConceptEngine.h"

#include "Core/Application/Game/CEGame.h"
#include "Core/Application/Editor/CEEditor.h"
#include "Core/Application/GameDebug/CEGameDebug.h"
#include "Core/Platform/CEPlatformActions.h"
#include "Core/Platform/Windows/CEWindows.h"
#include "Utilities/CEScreenUtilities.h"

ConceptEngine::ConceptEngine::ConceptEngine(std::wstring name,
                                            Graphics::Main::GraphicsAPI api,
                                            Core::Generic::Platform::Platform platform,
                                            Core::Compilers::Language language): EnumApi(api),
	EnumLanguage(language),
	EnumPlatform(platform) {
	Name = name;
	if (Core::Application::CECore::ShowConsole) {
		Create(EngineBoot::GameDebug);
	}
	else {
		Create(EngineBoot::Game);
	}
}

ConceptEngine::ConceptEngine::ConceptEngine(std::wstring name,
                                            Graphics::Main::GraphicsAPI api,
                                            Core::Generic::Platform::Platform platform) : EnumApi(api),
	EnumLanguage(Core::Compilers::Language::None),
	EnumPlatform(platform) {
	Name = name;
	Create(EngineBoot::Editor);
}

bool ConceptEngine::ConceptEngine::Init() const {
	if (!Core->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to Initialize Concept Engine");
		return false;
	}

	return true;
}

int ConceptEngine::ConceptEngine::Run() const {
	return Core->Run();
}

bool ConceptEngine::ConceptEngine::Release() {
	if (!Core->Release()) {
		CEPlatformActions::MessageBox("Error", "Failed to Release Concept Engine");
		return false;
	}

	return true;
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
	return Core.get();
}

std::wstring ConceptEngine::ConceptEngine::GetName() {
	return Name;
}

bool ConceptEngine::ConceptEngine::CreateEditor() {
	auto editor = std::make_unique<Core::Application::Editor::CEEditor>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(editor);
	return true;
}

bool ConceptEngine::ConceptEngine::CreateGame() {
	auto game = std::make_unique<Core::Application::Game::CEGame>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(game);
	return true;
}

bool ConceptEngine::ConceptEngine::CreateGameDebug() {
	auto gameDebug = std::make_unique<Core::Application::GameDebug::CEGameDebug>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(gameDebug);
	return true;
}
