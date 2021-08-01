#pragma once
#include "FrameResources.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/CEScene.h"

class RayTracer
{
public:
    RayTracer()  = default;
    ~RayTracer() = default;

    bool Init(FrameResources& Resources);
    void Release();

    void PreRender(CommandList& CmdList, FrameResources& Resources, const CEScene& Scene);

private:
    CERef<RayTracingPipelineState> Pipeline;
    CERef<RayGenShader>        RayGenShader;
    CERef<RayMissShader>       RayMissShader;
    CERef<RayClosestHitShader> RayClosestHitShader;
};