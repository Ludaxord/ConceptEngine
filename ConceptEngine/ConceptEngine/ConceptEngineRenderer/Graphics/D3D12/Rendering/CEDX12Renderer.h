#pragma once
#include "Rendering/CERenderer.h"

class CEDX12Renderer : public CERenderer {
public:
	bool Create() override;
	void Release() override;
	void PerformFrustumCulling(const CEScene& Scene) override;
	void PerformFXAA(CommandList& InCmdList) override;
	void PerformBackBufferBlit(CommandList& InCmdList) override;
	void PerformAABBDebugPass(CommandList& InCmdList) override;
	void RenderDebugInterface() override;
	void Update(const CEScene& Scene) override;
private:
	bool CreateBoundingBoxDebugPass() override;
	bool CreateAA() override;
	bool CreateShadingImage() override;
	void ResizeResources(uint32 Width, uint32 Height) override;
protected:
	void OnWindowResize(const WindowResizeEvent& Event) override;
};
