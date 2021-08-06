#include "ConceptEngine.h"

#include <chrono>

#include "Boot/CEDebugRuntime.h"
#include "Boot/CEEditor.h"
#include "Boot/CERuntime.h"

ConceptEngine::ConceptEngine(const std::wstring& Name,
                             ::GraphicsAPI GApi,
                             ::PlatformBoot PBoot,
                             ::ScriptingLanguage SLanguage,
                             EngineBoot EBoot,
                             PhysicsLibrary PLibrary,
                             bool ShowConsole): Core(nullptr), StartTime(0) {
	GEngineConfig = {GApi, SLanguage, PBoot, EBoot, PLibrary, Name, ShowConsole};
	SetStartTime();
}

ConceptEngine::ConceptEngine(const std::wstring& Name,
                             ::GraphicsAPI GApi,
                             ::PlatformBoot PBoot,
                             ::ScriptingLanguage SLanguage,
                             EngineBoot EBoot,
                             PhysicsLibrary PLibrary) : ConceptEngine(
	Name, GApi, PBoot, SLanguage, EBoot, PLibrary, false) {
}

ConceptEngine::ConceptEngine(const std::wstring& Name, ::GraphicsAPI GApi, ::PlatformBoot PBoot,
                             EngineBoot EBoot, PhysicsLibrary PLibrary) : ConceptEngine(
	Name, GApi, PBoot, ScriptingLanguage::None, EBoot, PLibrary) {
}

ConceptEngine::ConceptEngine(CEEngineConfig& EConfig): Core(nullptr), StartTime(0) {
	GEngineConfig = EConfig;
}

void ConceptEngine::Run() const {
	Core->Run();
}

bool ConceptEngine::Release() const {
	if (!Core->Release()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Release Concept Engine");
		return false;
	}

	return true;
}

CECore* ConceptEngine::GetCore() const {
	return Core;
}

bool ConceptEngine::Create() const {
	if (!Core->Create()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Create Concept Engine");
		return false;
	}

	return true;
}

bool ConceptEngine::CreateEditor() {
	Core = new CEEditor(GEngineConfig);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CreateRuntime() {
	Core = new CERuntime(GEngineConfig);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CreateDebugRuntime() {
	Core = new CEDebugRuntime(GEngineConfig);
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::SetStartTime() {
	StartTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	return true;
}
