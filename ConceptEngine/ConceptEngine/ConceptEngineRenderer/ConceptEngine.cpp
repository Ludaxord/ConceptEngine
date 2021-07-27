#include "ConceptEngine.h"

ConceptEngine::ConceptEngine(std::wstring Name,
                             ::GraphicsAPI GApi,
                             ::PlatformBoot PBoot,
                             ::ScriptingLanguage SLanguage) : GraphicsAPI(GApi),
                                                              PlatformBoot(PBoot),
                                                              ScriptingLanguage(SLanguage) {
	SetStartTime();
	InstanceName = Name;
}

ConceptEngine::ConceptEngine(std::wstring Name, ::GraphicsAPI GApi, ::PlatformBoot PBoot) {
}

bool ConceptEngine::Initialize() {
}

void ConceptEngine::Run() {
}

bool ConceptEngine::Release() {
}

CECore* ConceptEngine::GetCore() const {
}

std::string ConceptEngine::GetName() {
}

bool ConceptEngine::Create(EngineBoot Boot) {
}

bool ConceptEngine::CreateEditor() {
}

bool ConceptEngine::CreateRuntime() {
}

bool ConceptEngine::CreateDebugRuntime() {
}

bool ConceptEngine::SetStartTime() {
}
