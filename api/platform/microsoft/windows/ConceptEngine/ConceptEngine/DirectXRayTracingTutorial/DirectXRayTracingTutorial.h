#pragma once
#include "../DirectXFrameworkTutorial/Tutorial.h"

class DirectXRayTracingTutorial : public Tutorial {

public:
	DirectXRayTracingTutorial(UINT width, UINT height);

	void OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDestroy() override;
	void OnSizeChanged(UINT width, UINT height, bool minimized) override;
	void OnKeyDown(KeyCode key) override;
	void OnKeyUp(KeyCode key) override;
	void OnWindowMoved(int x, int y) override;
	void OnMouseMove(UINT x, UINT y) override;
	void OnMouseButtonDown(KeyCode key, UINT x, UINT y) override;
	void OnMouseButtonUp(KeyCode key, UINT x, UINT y) override;
	void OnDisplayChanged() override;
	wrl::ComPtr<IDXGISwapChain> GetSwapChain() override;
protected:
private:
};
