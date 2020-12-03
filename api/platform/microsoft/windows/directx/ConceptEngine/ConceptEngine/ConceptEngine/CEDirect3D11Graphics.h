#pragma once
#include "CEDirect3DGraphics.h"

class CEDirect3D11Graphics : public CEDirect3DGraphics {
public:
	CEDirect3D11Graphics(HWND hWnd);
	~CEDirect3D11Graphics() override = default;


public:
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;


private:
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<IDXGISwapChain> pSwap;
	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<ID3D11RenderTargetView> pTarget;
	wrl::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};
