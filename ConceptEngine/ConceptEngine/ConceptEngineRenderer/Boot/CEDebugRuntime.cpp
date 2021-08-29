#include "CEDebugRuntime.h"

#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"
#include "Platform/Generic/Console/CETypedConsole.h"
#include "Project/CEProject.h"
#include "Project/CEProjectSerializer.h"

#define TEST_RUNTIME 1

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

bool CEDebugRuntime::OpenProject() {
#if defined(TEST_RUNTIME)
	return true;
#endif

	CEProject* Project = DBG_NEW CEProject();
	CEProjectSerializer Serializer(Project);
	if (!Serializer.Deserialize(ProjectPath)) {
		CE_LOG_ERROR("[CERuntime]: Failed to Serialize Project");
		return false;
	}

	CEProject::SetActive(Project);

	//TODO: Create Scripting Engine and Load Assembly from loaded project file...

	//TODO: Load Scene from project file...


	return true;
}
