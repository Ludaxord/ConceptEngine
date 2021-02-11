#include <Windows.h>

#include "D3D12RaytracingHelloWorld.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    D3D12RaytracingHelloWorld sample(1280, 720, L"D3D12 Raytracing - Hello World");
    return Win32Application::Run(&sample, hInstance, nCmdShow);
}
