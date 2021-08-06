#include <Windows.h>
#include <crtdbg.h>

#include "../ConceptEngineRenderer/Windows/Windows.h"

#include "../ConceptEngineRenderer/Main/EngineMain.h"

#include "../ConceptEngineRenderer/Core/Application/Windows/CEWindowsPlatform.h"

#include "../ConceptEngineRenderer/Debug/CEDebug.h"

#include "../ConceptEngineRenderer/ConceptEngine.h"

#include "../ConceptEngineRenderer/Platform/Windows/CEWindows.h"
#pragma warning(push)
#pragma warning(disable : 4100) // Disable unreferenced variable

static void InitCRunTime() {
	uint32 DebugFlags = 0;
#ifdef DEBUG_BUILD
	DebugFlags |= _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF;
#endif

	_CrtSetDbgFlag(DebugFlags);
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow) {
	InitCRunTime();

	// CEWindowsPlatform::PreMainInit(Instance);

	//TODO: Change to ConceptEngineRenderer
	// return EngineMain();
	CEWindows::PreInit(Instance, CmdLine, NULL, CmdShow, 1920, 1080);

	return Exec(L"Concept Engine Runtime", GraphicsAPI::DirectX12, PlatformBoot::Windows, ScriptingLanguage::None,
	            EngineBoot::DebugRuntime, PhysicsLibrary::PhysX);
}

#pragma warning(pop)
