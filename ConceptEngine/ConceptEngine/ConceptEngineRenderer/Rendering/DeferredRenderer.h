#pragma once
#include "FrameResources.h"
#include "LightSetup.h"

#include "../Scene/Scene.h"

#include "../RenderLayer/CommandList.h"

class DeferredRenderer
{
public:
    DeferredRenderer()  = default;
    ~DeferredRenderer() = default;

    bool Init(FrameResources& FrameResources);
    void Release();

    void RenderPrePass(CommandList& CmdList, const FrameResources& FrameResources);
    void RenderBasePass(CommandList& CmdList, const FrameResources& FrameResources);
    void RenderDeferredTiledLightPass(CommandList& CmdList, const FrameResources& FrameResources, const LightSetup& LightSetup);

    bool ResizeResources(FrameResources& FrameResources);

private:
    bool CreateGBuffer(FrameResources& FrameResources);

    CERef<GraphicsPipelineState> PipelineState;
    CERef<VertexShader>          BaseVertexShader;
    CERef<PixelShader>           BasePixelShader;
    CERef<GraphicsPipelineState> PrePassPipelineState;
    CERef<VertexShader>          PrePassVertexShader;
    CERef<ComputePipelineState>  TiledLightPassPSO;
    CERef<ComputeShader>         TiledLightShader;
    CERef<ComputePipelineState>  TiledLightPassPSODebug;
    CERef<ComputeShader>         TiledLightDebugShader;
};