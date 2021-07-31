#pragma once
#include "FrameResources.h"
#include "LightSetup.h"

#include "../RenderLayer/CommandList.h"

class LightProbeRenderer
{
public:
    LightProbeRenderer()  = default;
    ~LightProbeRenderer() = default;

    bool Init(LightSetup& LightSetup, FrameResources& FrameResources);
    
    void Release();

    void RenderSkyLightProbe(CommandList& CmdList, const LightSetup& LightSetup, const FrameResources& Resources);

private:
    bool CreateSkyLightResources(LightSetup& LightSetup);

    CERef<ComputePipelineState> IrradianceGenPSO;
    CERef<ComputeShader>        IrradianceGenShader;
    CERef<ComputePipelineState> SpecularIrradianceGenPSO;
    CERef<ComputeShader>        SpecularIrradianceGenShader;
};