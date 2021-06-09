#include <Windows.h>

#include "CEEngine.h"
#include "Core/Platform/Windows/CEWindows.h"
#include "Render/Scene/CECamera.h"

using namespace ConceptEngine;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Core::Platform::Windows::CEWindows::PreInit(hInstance, lpCmdLine, NULL, nCmdShow, 1920, 1080, true);

	CEEngine* cp = new CEEngine(L"Concept Debug Game", GraphicsAPI::DirectX, Core::Generic::Platform::Platform::Windows,
	                            Core::Compilers::Language::None);

	return RuntimeRun(cp);

}
