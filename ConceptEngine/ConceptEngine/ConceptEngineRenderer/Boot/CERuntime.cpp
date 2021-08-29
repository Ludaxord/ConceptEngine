#include "CERuntime.h"

#include "Debug/CEProfiler.h"
#include "Graphics/CEGraphics.h"
#include "Platform/CEPlatform.h"
#include "Project/CEProject.h"
#include "Project/CEProjectSerializer.h"

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

bool CERuntime::OpenProject() {
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
