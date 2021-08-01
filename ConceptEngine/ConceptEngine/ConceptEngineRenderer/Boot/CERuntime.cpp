#include "CERuntime.h"

#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"

CERuntime::CERuntime(CEEngineConfig& EConfig): CERuntime(EConfig, nullptr) {
}

CERuntime::CERuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground) : CECoreRuntime(EConfig, InPlayground) {
}

bool CERuntime::Create() {
	if (!CECoreRuntime::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CERuntime::Update(CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		CEProfiler::Update();
	});
}
