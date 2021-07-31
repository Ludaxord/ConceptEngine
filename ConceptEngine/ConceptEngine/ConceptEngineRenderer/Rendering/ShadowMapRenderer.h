#pragma once
#include "FrameResources.h"
#include "LightSetup.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/Scene.h"

class ShadowMapRenderer
{
public:
    ShadowMapRenderer()  = default;
    ~ShadowMapRenderer() = default;

    bool Init(LightSetup& LightSetup, FrameResources& Resources);

    void RenderPointLightShadows(CommandList& CmdList, const LightSetup& LightSetup, const Scene& Scene);
    void RenderDirectionalLightShadows(CommandList& CmdList, const LightSetup& LightSetup, const Scene& Scene);

    void Release();

private:
    bool CreateShadowMaps(LightSetup& FrameResources);

    CERef<ConstantBuffer> PerShadowMapBuffer;

    CERef<GraphicsPipelineState> DirLightPipelineState;
    CERef<VertexShader>          DirLightShader;
    CERef<GraphicsPipelineState> PointLightPipelineState;
    CERef<VertexShader>          PointLightVertexShader;
    CERef<PixelShader>           PointLightPixelShader;

    bool   UpdateDirLight   = true;
    bool   UpdatePointLight = true;
    uint64 DirLightFrame    = 0;
    uint64 PointLightFrame  = 0;
};