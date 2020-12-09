#pragma once
#include "CEGraphics.h"
#include "CEInfoManager.h"

class CEDirect3DGraphics : public CEGraphics {
public:
	CEDirect3DGraphics(HWND hWnd, CEGraphicsApiTypes apiType);

	void EndFrame() override;
	void ClearBuffer(float red, float green, float blue, float alpha) noexcept override;
	void DrawDefaultFigure(float angle, float windowWidth, float windowHeight, float x, float y, float z,
		CEDefaultFigureTypes figureTypes) override;
protected:
#ifndef NDEBUG
	CEInfoManager infoManager;
#endif
};
