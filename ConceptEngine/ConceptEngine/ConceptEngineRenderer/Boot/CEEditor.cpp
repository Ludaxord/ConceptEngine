#include "CEEditor.h"

#include "Callbacks/CEEngineController.h"
#include "Core/Threading/CETaskManager.h"
#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"
#include "Platform/Generic/Console/CETypedConsole.h"
#include "Project/CEPlayground.h"
#include "Time/CETimer.h"

CEEditor::CEEditor(CEEngineConfig& EConfig): CEEditor(EConfig, nullptr) {
}

CEEditor::CEEditor(CEEngineConfig& EConfig, CEPlayground* InPlayground): CECore(EConfig, InPlayground) {
}

bool CEEditor::Create() {
	if (!CECore::Create()) {
		CEDebug::DebugBreak();
		return false;
	}
	return true;
}

void CEEditor::Run() {
	Timer = CETimer();
}

void CEEditor::Update(CETimestamp DeltaTime) {
	GPlatform->Update();
	GGraphics->Update(DeltaTime, [] {
		CEProfiler::Update();
	});
}

bool CEEditor::Release() {
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
