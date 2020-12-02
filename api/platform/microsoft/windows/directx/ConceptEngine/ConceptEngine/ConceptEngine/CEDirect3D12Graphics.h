#pragma once
#include "CEGraphics.h"

class CEDirect3D12Graphics : public CEGraphics {

public:
	CEDirect3D12Graphics(HWND hWnd);
	~CEDirect3D12Graphics() = default;

public:
	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
	                       CEDefaultFigureTypes figureTypes) override;
};
