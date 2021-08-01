#include "CEDebugRuntime.h"

#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"
#include "Platform/Generic/Console/CETypedConsole.h"

CEDebugRuntime::CEDebugRuntime(CEEngineConfig& EConfig): CEDebugRuntime(EConfig, nullptr) {
}

CEDebugRuntime::CEDebugRuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground) : CECoreRuntime(
	EConfig, InPlayground) {
}

bool CEDebugRuntime::Create() {
	if (!GPlatform->CreateSystemConsole()) {
		CEPlatformMisc::MessageBox("Error", "Failed to Create System Debug Console");
		return false;
	}

	CEPlatform::GetConsole()->SetTitle("Concept Engine Events Console");
	if (!CEPlatform::GetConsole()->Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CECoreRuntime::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEDebugRuntime::Update(CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		CEPlatform::GetConsole()->Update();
		GTypedConsole.Update();
		CEProfiler::Update();
	});
}
