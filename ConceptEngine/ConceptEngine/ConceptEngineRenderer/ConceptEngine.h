#pragma once
#include "Config/CEGlobalConfigs.h"

class CECore;
class CEPlatform;

class ConceptEngine {
public:
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	              EngineBoot EBoot,
	              bool ShowConsole);
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	              EngineBoot EBoot);
	ConceptEngine(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, EngineBoot EBoot);
	ConceptEngine(CEEngineConfig& EConfig);

	bool Create();
	void Run();
	bool Release();

	CECore* GetCore() const;

	 CEEngineConfig GetEngineConfig() const;

protected:
	bool CreateEditor();
	bool CreateRuntime();
	bool CreateDebugRuntime();

	bool SetStartTime();

private:
	friend class CEPlatform;
	friend int Exec(const std::wstring& Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage,
	                EngineBoot EBoot);
	CECore* Core;

	CEEngineConfig EngineConfig;
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
                EngineBoot EBoot) {
	ConceptEngine* Engine = new ConceptEngine(Name, GApi, PBoot, SLanguage, EBoot);
	switch (Engine->EngineConfig.EngineBoot) {
	case EngineBoot::Runtime:
		Engine->CreateRuntime();
		break;
	case EngineBoot::Editor:
		Engine->CreateEditor();
		break;
	case EngineBoot::DebugRuntime:
		Engine->CreateDebugRuntime();
		break;
	default:
		throw new CEException();
	}

	return EngineExec(Engine);
}
