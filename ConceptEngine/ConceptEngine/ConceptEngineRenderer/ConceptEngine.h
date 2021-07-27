#pragma once

enum class GraphicsAPI;
enum class EngineBoot;
enum class PlatformBoot;
enum class ScriptingLanguage;

class CECore;
class CEPlatform;

class ConceptEngine {
public:
	ConceptEngine(std::wstring Name, GraphicsAPI GApi, PlatformBoot PBoot, ScriptingLanguage SLanguage);
	ConceptEngine(std::wstring Name, GraphicsAPI GApi, PlatformBoot PBoot);

	bool Initialize();
	void Run();
	bool Release();

	CECore* GetCore() const;

	static std::string GetName();

protected:
	bool Create(EngineBoot Boot);

	bool CreateEditor();
	bool CreateRuntime();
	bool CreateDebugRuntime();

	bool SetStartTime();

private:
	friend class CEPlatform;
	CECore* Core;

	//TODO: Move config values to EngineConfig
	GraphicsAPI GraphicsAPI;
	ScriptingLanguage ScriptingLanguage;
	PlatformBoot PlatformBoot;

	std::wstring InstanceName;
	std::time_t StartTime;
};

inline int EngineExec(ConceptEngine* Engine) {
	if (!Engine->Initialize()) {
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
