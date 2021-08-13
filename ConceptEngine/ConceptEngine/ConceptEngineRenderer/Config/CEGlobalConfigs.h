#pragma once
#include <Windows.h>

enum class EngineBoot {
	Runtime,
	DebugRuntime,
	Editor,
	None
};

enum class PhysicsLibrary {
	PhysX,
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
	                  EngineBoot(EngineBoot::None),
	                  PhysicsLibrary(PhysicsLibrary::None) {

	}

	CEEngineConfig(GraphicsAPI GApi, ScriptingLanguage SLanguage, PlatformBoot PBoot, EngineBoot EBoot,
	               PhysicsLibrary PLibrary,
	               const std::wstring& InstName,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(SLanguage),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		InstanceName(InstName),
		ShowConsole(bShowConsole),
		PhysicsLibrary(PLibrary) {
	}

	CEEngineConfig(GraphicsAPI GApi, PlatformBoot PBoot, EngineBoot EBoot,
	               PhysicsLibrary PLibrary, const std::wstring& InstName,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(ScriptingLanguage::None),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		InstanceName(InstName),
		ShowConsole(bShowConsole),
		PhysicsLibrary(PLibrary) {
	}

	CEEngineConfig(GraphicsAPI GApi, ScriptingLanguage SLanguage, PlatformBoot PBoot, EngineBoot EBoot,
	               PhysicsLibrary PLibrary,
	               bool bShowConsole = false):
		GraphicsAPI(GApi),
		ScriptingLanguage(SLanguage),
		PlatformBoot(PBoot),
		EngineBoot(EBoot),
		ShowConsole(bShowConsole),
		PhysicsLibrary(PLibrary) {
	}

	GraphicsAPI GraphicsAPI;
	ScriptingLanguage ScriptingLanguage;
	PlatformBoot PlatformBoot;
	EngineBoot EngineBoot;
	PhysicsLibrary PhysicsLibrary;

	std::wstring InstanceName;

	bool ShowConsole = false;
};

struct CEProjectConfig {
public:
	std::wstring PTitle;
	HINSTANCE PInstance = NULL;
	LPSTR PCmdLine = NULL;
	HWND PHWnd = NULL;
	int PCmdShow = 0;

	std::string GetStringTitle() {
		return std::string(PTitle.begin(), PTitle.end());
	}
};

struct CEPhysicsConfig {
	
};

inline CEEngineConfig GEngineConfig;
