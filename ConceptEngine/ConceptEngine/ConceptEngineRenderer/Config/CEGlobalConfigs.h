#pragma once

enum class EngineBoot {
	Runtime,
	DebugRuntime,
	Editor,
	None
};

enum class PlatformBoot {
	Windows,
	Mac,
	Linux,
	Android,
	iOS,
	None
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
	WebGL,
	None
};

struct CEEngineConfig {
	CEEngineConfig(): GraphicsAPI(GraphicsAPI::None),
	                  ScriptingLanguage(ScriptingLanguage::None),
	                  PlatformBoot(PlatformBoot::None),
	                  EngineBoot(EngineBoot::None) {

	}

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
	HINSTANCE Instance;
	LPSTR CmdLine;
	HWND HWnd;
	int CmdShow;

	std::string GetStringTitle() {
		return std::string(Title.begin(), Title.end());
	}
};

inline CEEngineConfig GEngineConfig;
