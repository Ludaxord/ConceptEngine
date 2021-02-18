#include <Windows.h>

#include "../RTHelloWorld/D3D12RaytracingHelloWorld.h"
#include "../RTHelloWorld/Win32Application.h"
#include "../RTLibrarySubObjects/D3D12RaytracingLibrarySubObjects.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	{
		D3D12RaytracingLibrarySubObjects libSubObjects{};
		D3D12RaytracingHelloWorld sample(1920, 1080, L"D3D12 Raytracing - Hello World");
		return Win32Application::Run(&sample, hInstance, nCmdShow);
	}
}
