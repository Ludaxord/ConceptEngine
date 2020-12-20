#pragma once
#include "CEGraphics.h"

class CEMetalGraphics : public CEGraphics {
public:
	CEMetalGraphics(HWND hWnd);
	CEGraphicsManager GetGraphicsManager() override;
	void PrintGraphicsVersion() override;
	void OnInit() override;
	void OnDestroy() override;
	void OnRender() override;
	void OnUpdate() override;
};
