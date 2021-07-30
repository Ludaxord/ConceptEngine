#pragma once
#include "Rendering/CERenderer.h"

class CEDXRenderer : public CERenderer {
public:
	bool Create() override;
	void Release() override;
	void PerformFrustumCulling(const Scene& Scene) override;
	void PerformFXAA(CommandList& InCmdList) override;
	void PerformBackBufferBlit(CommandList& InCmdList) override;
	void PerformAABBDebugPass(CommandList& InCmdList) override;
	void RenderDebugInterface() override;
	void Update(const Scene& Scene) override;
private:
	bool CreateBoundingBoxDebugPass() override;
	bool CreateAA() override;
	bool CreateShadingImage() override;
	void ResizeResources(uint32 Width, uint32 Height) override;
};
