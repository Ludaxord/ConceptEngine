#include "CECoreRuntime.h"

#include "Callbacks/CEEngineController.h"
#include "Core/Threading/CETaskManager.h"
#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"
#include "Platform/Generic/Console/CETypedConsole.h"
#include "Project/CEPlayground.h"

CECoreRuntime::CECoreRuntime(CEEngineConfig& EConfig): CECoreRuntime(EConfig, nullptr) {
}

CECoreRuntime::CECoreRuntime(CEEngineConfig& EConfig, CEPlayground* InPlayground) : CECore(EConfig, InPlayground) {
}

bool CECoreRuntime::Create() {
	if (!GPlatform->CreateSystemWindow()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!GPlatform->CreateCursor()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CECore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!GPlatform->CreateManagers()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CECoreRuntime::Run() {
	CETimer Timer;
	while(bIsRunning) {
		Timer.Update();
		Update(Timer.GetDeltaTime());
	}
}

bool CECoreRuntime::Release() {
	TRACE_FUNCTION_SCOPE();

	GCmdListExecutor.WaitForGPU();

	CastTextureManager()->Release();

	if (!GPlayground->Release()) {
		return false;
	}

	if (GEngineController.Release()) {
		GPlatform->SetCallbacks(nullptr);
	}
	else {
		return false;
	}

	CEGraphics::GetDebugUI()->Release();

	GGraphics->Release();

	CETaskManager::Get().Release();

	if (!GPlatform->Release()) {
		return false;
	}

	return true;}
