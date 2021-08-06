#pragma once
#include "Config/CEGlobalConfigs.h"
#include "Exception/CEException.h"
#include "Core/Application/Log.h"

#include <memory>
#include <string>
#include <ctime>

class CECore;
class CEPlatform;

class ConceptEngine {
public:
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	              EngineBoot EBoot,
	              PhysicsLibrary PLibrary,
	              bool ShowConsole);
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	              EngineBoot EBoot,
	              PhysicsLibrary PLibrary);
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, EngineBoot EBoot,
	              PhysicsLibrary PLibrary);
	ConceptEngine(CEEngineConfig& EConfig);

	bool Create() const;
	void Run() const;
	bool Release() const;

	CECore* GetCore() const;

protected:
	friend class CEPlatform;
	friend int Exec(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	                EngineBoot EBoot, PhysicsLibrary PLibrary);
	bool CreateEditor();
	bool CreateRuntime();
	bool CreateDebugRuntime();

	bool SetStartTime();

private:
	CECore* Core;

	std::time_t StartTime;
};

inline int EngineExec(ConceptEngine* Engine) {
	if (!Engine->Create()) {
		CEPlatformMisc::MessageBox("Error", "Concept Engine Initialize Failed");
		return -1;
	}

	Engine->Run();

	if (!Engine->Release()) {
		CEPlatformMisc::MessageBox("Error", "Concept Engine Release Failed");
		return -1;
	}

	return 0;
}

inline int Exec(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
                EngineBoot EBoot, PhysicsLibrary PLibrary) {

	auto Engine = new ConceptEngine(Name, GApi, PBoot, SLanguage, EBoot, PLibrary);
	switch (GEngineConfig.EngineBoot) {
	case EngineBoot::Runtime:
		if (!Engine->CreateRuntime()) {
			CE_LOG_ERROR("[ConceptEngine]: Failed to create Runtime");
			return false;
		}
		break;
	case EngineBoot::Editor:
		if (!Engine->CreateEditor()) {
			CE_LOG_ERROR("[ConceptEngine]: Failed to create Editor");
			return false;
		}
		break;
	case EngineBoot::DebugRuntime:
		if (!Engine->CreateDebugRuntime()) {
			CE_LOG_ERROR("[ConceptEngine]: Failed to create Debug Runtime");
			return false;
		}
		break;
	default:
		throw CEEngineBootException();
	}

	return EngineExec(Engine);
}
