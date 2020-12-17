#pragma once
#include "CEGraphics.h"
#include "CEInfoManager.h"

#include <d3d11.h>
#include "CEDirect3D11Manager.h"
#include "CEOSTools.h"

#pragma comment(lib, "d3d11.lib")

// #include <d3d12.h>
// #include <dxgi1_6.h>
// #include <DirectXMath.h>
//
// #include "CED3D12Helper.h"

// #pragma comment(lib, "d3d12.lib")
// #pragma comment(lib, "dxgi.lib")
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "CED3D12Helper.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <filesystem>
// #include <boost/filesystem.hpp>
// #pragma comment(lib, "libboost_filesystem-vc142-mt-gd-x64-1_76.lib")

using namespace DirectX;
namespace fs = std::filesystem;


class CEDirect3DGraphics : public CEGraphics {
public:
	// explicit CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType);
	CEDirect3DGraphics(HWND hWnd, CEOSTools::CEGraphicsApiTypes apiType, int width, int height);

	// void EndFrame() override;
	// void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	// void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	//                        CEDefaultFigureTypes figureTypes) override;
	CEGraphicsManager GetGraphicsManager() override;

private:
	void CreateDirect3D12(HWND hWnd, int width, int height);
	void CreateDirect3D11(HWND hWnd, int width, int height);
public:
	void PrintGraphicsVersion() override;
protected:
#ifndef NDEBUG
	CEInfoManager infoManager;
#endif
private:
	CEDirect3D11Manager pManager;


	//TODO: REFACTOR ALL OF THIS, right now is just to enable direct3D 12
protected:
	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);
public:
	void OnRender() override;

};
