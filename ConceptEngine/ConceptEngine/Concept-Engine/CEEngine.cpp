#include "CEEngine.h"

#include "Core/Debug/CEDebug.h"


ConceptEngine::CEEngine::CEEngine(std::wstring name,
                                            GraphicsAPI api,
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

ConceptEngine::CEEngine::CEEngine(std::wstring name,
                                            GraphicsAPI api,
                                            Core::Generic::Platform::Platform platform) : EnumApi(api),
	EnumLanguage(Core::Compilers::Language::None),
	EnumPlatform(platform) {
	Name = name;
	Create(EngineBoot::Editor);
}

bool ConceptEngine::CEEngine::Init() const {
	if (!Core->Create()) {
		CEPlatformActions::MessageBox("Error", "Failed to Initialize Concept Engine");
		return false;
	}

	return true;
}

void ConceptEngine::CEEngine::Run() const {
	return Core->Run();
}

bool ConceptEngine::CEEngine::Release() {
	if (!Core->Release()) {
		CEPlatformActions::MessageBox("Error", "Failed to Release Concept Engine");
		return false;
	}

	return true;
}

bool ConceptEngine::CEEngine::Create(EngineBoot boot) {
	EnumEngineBoot = boot;
	
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

ConceptEngine::Core::Application::CECore* ConceptEngine::CEEngine::GetCore() const {
	return Core.get();
}

std::wstring ConceptEngine::CEEngine::GetName() {
	return Name;
}

ConceptEngine::EngineBoot ConceptEngine::CEEngine::GetEngineBoot() {
	return EnumEngineBoot;
}

bool ConceptEngine::CEEngine::CreateEditor() {
	auto editor = std::make_unique<Core::Application::Editor::CEEditor>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(editor);
	Core::Debug::CEDebug::DebugBreak();
	return true;
}

bool ConceptEngine::CEEngine::CreateGame() {
	auto game = std::make_unique<Core::Application::Game::CEGame>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(game);
	Core::Debug::CEDebug::DebugBreak();
	return true;
}

bool ConceptEngine::CEEngine::CreateGameDebug() {
	auto gameDebug = std::make_unique<Core::Application::GameDebug::CEGameDebug>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(gameDebug);
	Core::Debug::CEDebug::DebugBreak();
	return true;
}
