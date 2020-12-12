#pragma once
#include "CEGraphics.h"
#include "CEInfoManager.h"

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <winrt/base.h>
#include "CEDirect3D11Manager.h"

#pragma comment(lib, "d3d11.lib")


class CEDirect3DGraphics : public CEGraphics {
public:
	CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType);

	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;

	void CreateDirect3D12();
	void CreateDirect3D11();
	CEGraphicsManager GetGraphicsManager() override;
protected:
#ifndef NDEBUG
	CEInfoManager infoManager;
#endif
private:
	CEDirect3D11Manager pManager;

};
