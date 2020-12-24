#pragma once
#include "CEGraphics.h"

class CEMetalGraphics : public CEGraphics {
public:
	CEMetalGraphics(HWND hWnd);
	CEManager GetGraphicsManager() override;
	void PrintGraphicsVersion() override;
	bool OnInit() override;
	void OnDestroy() override;
	void OnRender() override;
	void OnUpdate() override;
};
