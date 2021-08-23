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

enum class PhysicsFrictionType {
	Patch,
	OneDirectional,
	TwoDirectional
};

enum class PhysicsBroadPhaseType {
	SweepAndPrune,
	MultiBoxPrune,
	AutomaticBoxPrune
};

enum class PhysicsDebugType {
	DebugToFile = 0,
	LiveDebug = 1
};

struct CEPhysicsConfig {
	CETimestamp FixedTimeStep = 1.0f / 100.0f;
	XMFLOAT3 Gravity = {0.0f, -9.81f, 0.0f};
	PhysicsBroadPhaseType BroadPhaseAlgorithm = PhysicsBroadPhaseType::AutomaticBoxPrune;
	XMFLOAT3 WorldBoundsMin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 WorldBoundsMax = XMFLOAT3(1.0f, 1.0f, 1.0f);
	uint32 WorldBoundsSubdivisions = 2;
	PhysicsFrictionType FrctionModel = PhysicsFrictionType::Patch;
	uint32 SolverIterations = 6;
	uint32 SolverVelocityIterations = 1;
	bool DebugOnPlay = true;
	PhysicsDebugType DebugType = PhysicsDebugType::LiveDebug;
};

inline CEEngineConfig GEngineConfig;
// inline CEPhysicsConfig GPhysicsConfig;
