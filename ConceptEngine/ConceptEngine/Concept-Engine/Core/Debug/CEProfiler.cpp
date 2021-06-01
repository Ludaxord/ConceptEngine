#include "CEProfiler.h"

#include "../Platform/Generic/Debug/CETypedConsole.h"
#include "../Platform/Generic/Debug/CEConsoleVariable.h"

constexpr float MICROSECONDS = 1000.0f;
constexpr float MILLISECONDS = 1000.0f * 1000.0f;
constexpr float SECONDS = 1000.0f * 1000.0f * 1000.0f;
constexpr float INV_MICROSECONDS = 1.0f / MICROSECONDS;
constexpr float INV_MILLISECONDS = 1.0f / MILLISECONDS;
constexpr float INV_SECONDS = 1.0f / SECONDS;

constexpr float MAX_FRAMETIME_MS = 1000.0f / 30.0f;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<bool> ShowFPS(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<bool> ShowProfiler(false);

void ConceptEngine::Core::Debug::CEProfiler::Create() {
	INIT_CONSOLE_VARIABLE("CE.FPS", &ShowFPS);
	INIT_CONSOLE_VARIABLE("CE.Profiler", &ShowProfiler);
}

void ConceptEngine::Core::Debug::CEProfiler::Update() {
}

void ConceptEngine::Core::Debug::CEProfiler::Enable() {
}

void ConceptEngine::Core::Debug::CEProfiler::Disable() {
}

void ConceptEngine::Core::Debug::CEProfiler::Reset() {
}

void ConceptEngine::Core::Debug::CEProfiler::BeginTraceScope(const char* Name) {
}

void ConceptEngine::Core::Debug::CEProfiler::EndTraceScope(const char* Name) {
}

void ConceptEngine::Core::Debug::CEProfiler::BeginGPUFrame(CECommandList& commandList) {
}

void ConceptEngine::Core::Debug::CEProfiler::BeginGPUTrace(CECommandList& commandList, const char* Name) {
}

void ConceptEngine::Core::Debug::CEProfiler::EndGPUFrame(CECommandList& commandList) {
}

void ConceptEngine::Core::Debug::CEProfiler::EndGPUTrace(CECommandList& commandList, const char* Name) {
}

void ConceptEngine::Core::Debug::CEProfiler::SetGPUProfiler(CEGPUProfiler* Profiler) {
}
