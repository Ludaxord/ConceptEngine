#pragma once

enum class EngineBoot {
	Runtime,
	DebugRuntime,
	Editor
};

enum class PlatformBoot {
	Windows,
	Mac,
	Linux,
	Android,
	iOS
};

enum class ScriptingLanguage {
	Graphs,
	CSharp,
	Cpp,
	Python,
	JavaScript,
	None
};

enum class GraphicsAPI {
	DirectX12,
	DirectX11,
	Vulkan,
	OpenGL,
	Metal,
	WebGL
};

struct CEEngineConfig {
	CEEngineConfig(GraphicsAPI GApi, ScriptingLanguage SLanguage, PlatformBoot PBoot, EngineBoot EBoot,
	               const std::wstring& InstName,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(SLanguage),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		InstanceName(InstName),
		ShowConsole(bShowConsole) {
	}

	CEEngineConfig(GraphicsAPI GApi, PlatformBoot PBoot, EngineBoot EBoot, const std::wstring& InstName,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(ScriptingLanguage::None),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		InstanceName(InstName),
		ShowConsole(bShowConsole) {
	}

	CEEngineConfig(GraphicsAPI GApi, ScriptingLanguage SLanguage, PlatformBoot PBoot, EngineBoot EBoot,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(SLanguage),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		ShowConsole(bShowConsole) {
	}

	GraphicsAPI GraphicsAPI;
	ScriptingLanguage ScriptingLanguage;
	PlatformBoot PlatformBoot;
	EngineBoot EngineBoot;

	std::wstring InstanceName;

	bool ShowConsole = false;
};

struct CEProjectConfig {
	std::wstring Title;
};
