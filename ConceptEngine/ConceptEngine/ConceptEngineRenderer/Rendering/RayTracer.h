#pragma once
#include "FrameResources.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/Scene.h"

class RayTracer
{
public:
    RayTracer()  = default;
    ~RayTracer() = default;

    bool Init(FrameResources& Resources);
    void Release();

    void PreRender(CommandList& CmdList, FrameResources& Resources, const Scene& Scene);

private:
    CERef<RayTracingPipelineState> Pipeline;
    CERef<RayGenShader>        RayGenShader;
    CERef<RayMissShader>       RayMissShader;
    CERef<RayClosestHitShader> RayClosestHitShader;
};