#include "CEEngine.h"

#include <chrono>

#include "Core/Application/CECore.h"
#include "Core/Application/Editor/CEEditor.h"
#include "Core/Application/Game/CEGame.h"
#include "Core/Application/GameDebug/CEGameDebug.h"
#include "Core/Debug/CEDebug.h"
#include "Utilities/CEUtilities.h"

ConceptEngine::CEEngine::CEEngine(std::wstring name,
                                  GraphicsAPI api,
                                  Core::Generic::Platform::Platform platform,
                                  Core::Compilers::Language language): EnumApi(api),
                                                                       EnumLanguage(language),
                                                                       EnumPlatform(platform) {
	SetStartTime();
	InstanceName = name;
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
	SetStartTime();
	InstanceName = name;
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
	CurrentBoot = boot;

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
	return InstanceName;
}

bool ConceptEngine::CEEngine::CreateEditor() {
	auto editor = std::make_unique<Core::Application::Editor::CEEditor>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(editor);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CEEngine::CreateGame() {
	auto game = std::make_unique<Core::Application::Game::CEGame>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(game);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CEEngine::CreateGameDebug() {
	auto gameDebug = std::make_unique<Core::Application::GameDebug::CEGameDebug>(EnumApi, EnumLanguage, EnumPlatform);
	Core = std::move(gameDebug);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CEEngine::SetStartTime() {
	StartTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	return true;
}
