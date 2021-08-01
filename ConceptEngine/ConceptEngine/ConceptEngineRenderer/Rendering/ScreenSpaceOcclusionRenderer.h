#pragma once
#include "FrameResources.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/CEScene.h"

class ScreenSpaceOcclusionRenderer
{
public:
    ScreenSpaceOcclusionRenderer()  = default;
    ~ScreenSpaceOcclusionRenderer() = default;

    bool Init(FrameResources& FrameResources);
    void Release();

    void Render(CommandList& CmdList, FrameResources& FrameResources);

    bool ResizeResources(FrameResources& FrameResources);

private:
    bool CreateRenderTarget(FrameResources& FrameResources);

    CERef<ComputePipelineState> PipelineState;
    CERef<ComputeShader>        SSAOShader;
    CERef<ComputePipelineState> BlurHorizontalPSO;
    CERef<ComputeShader>        BlurHorizontalShader;
    CERef<ComputePipelineState> BlurVerticalPSO;
    CERef<ComputeShader>        BlurVerticalShader;
    CERef<StructuredBuffer>     SSAOSamples;
    CERef<ShaderResourceView>   SSAOSamplesSRV;
    CERef<Texture2D>            SSAONoiseTex;
};