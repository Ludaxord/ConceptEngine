#pragma once
#include "CEGraphics.h"

class CEMetalGraphics : public CEGraphics {
public:
	CEMetalGraphics(HWND hWnd, int width, int height);
	void PrintGraphicsVersion() override;
	bool OnInit() override;
	void OnDestroy() override;
	void OnRender() override;
	void OnUpdate() override;
	bool LoadContent() override;
	void UnloadContent() override;
	void UpdatePerSecond(float second) override;
protected:
	void OnKeyPressed() override;
	void OnKeyReleased() override;
	void OnMouseMoved() override;
	void OnMouseButtonPressed() override;
	void OnMouseButtonReleased() override;
	void OnMouseWheel() override;
	void OnResize() override;
	void OnWindowDestroy() override;
	IGPUInfo GetGPUInfo() override;

public:
	void SetFullscreen(bool fullscreen) override;

	virtual void LoadBonus() override;
protected:
	void InitGui() override;
	void RenderGui() override;
	void DestroyGui() override;
};
