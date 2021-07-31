#pragma once
#include "FrameResources.h"
#include "LightSetup.h"

#include "../RenderLayer/CommandList.h"

class ForwardRenderer
{
public:
    ForwardRenderer()  = default;
    ~ForwardRenderer() = default;

    bool Init(FrameResources& FrameResources);
    void Release();

    void Render(CommandList& CmdList, const FrameResources& FrameResources, const LightSetup& LightSetup);

private:
    CERef<GraphicsPipelineState> PipelineState;
    CERef<VertexShader>          VShader;
    CERef<PixelShader>           PShader;
};