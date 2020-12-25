#pragma once
#include "CEManager.h"

class CEMetalManager : public CEManager {
public:
	using super = CEManager;
	CEMetalManager(const std::wstring& name, int width, int height, bool vSync);
protected:
	CEDevice GetDevice() override;
	virtual bool LoadContent() override;
	virtual void UnloadContent() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnKeyPressed() override;
	virtual void OnMouseWheel() override;
	virtual void OnResize() override;
public:
	virtual bool Initialize() override;
	virtual void Destroy() override;
protected:
	virtual void OnKeyReleased() override;
	virtual void OnMouseMoved() override;
	virtual void OnMouseButtonPressed() override;
	virtual void OnMouseButtonReleased() override;
	virtual void OnWindowDestroy() override;
};