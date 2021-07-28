#include "ConceptEngine.h"

#include <chrono>

ConceptEngine::ConceptEngine(const std::wstring& Name,
                             ::GraphicsAPI GApi,
                             ::PlatformBoot PBoot,
                             ::ScriptingLanguage SLanguage, EngineBoot EBoot,
                             bool ShowConsole) : EngineConfig(GApi, SLanguage, PBoot, EBoot, Name, ShowConsole) {
	SetStartTime();
}

ConceptEngine::ConceptEngine(const std::wstring& Name,
                             ::GraphicsAPI GApi,
                             ::PlatformBoot PBoot,
                             ::ScriptingLanguage SLanguage, EngineBoot EBoot) : ConceptEngine(
	Name, GApi, PBoot, SLanguage, EBoot, false) {
}

ConceptEngine::ConceptEngine(const std::wstring& Name, ::GraphicsAPI GApi, ::PlatformBoot PBoot,
                             EngineBoot EBoot) : ConceptEngine(Name, GApi, PBoot, ScriptingLanguage::None, EBoot) {
}

ConceptEngine::ConceptEngine(CEEngineConfig& EConfig) : EngineConfig(EConfig) {
}

void ConceptEngine::Run() {
}

bool ConceptEngine::Release() {
}

CECore* ConceptEngine::GetCore() const {
	return Core;
}

CEEngineConfig ConceptEngine::GetEngineConfig() const {
	return EngineConfig;
}

bool ConceptEngine::Create() {
}

bool ConceptEngine::CreateEditor() {
	Core = new CEEditor();
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CreateRuntime() {
	Core = new CERuntime();
	if (!Core) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

bool ConceptEngine::CreateDebugRuntime() {
	Core = new CEDebugRuntime();
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
