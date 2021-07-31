#pragma once
#include "FrameResources.h"

#include "../Rendering/Resources/MeshFactory.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/Scene.h"

class SkyboxRenderPass final
{
public:
    SkyboxRenderPass()  = default;
    ~SkyboxRenderPass() = default;

    bool Init(FrameResources& FrameResources);

    void Render(CommandList& CmdList, const FrameResources& FrameResources, const Scene& Scene);

    void Release();

private:
    CERef<GraphicsPipelineState> PipelineState;
    CERef<VertexShader> SkyboxVertexShader;
    CERef<PixelShader>  SkyboxPixelShader;
    CERef<VertexBuffer> SkyboxVertexBuffer;
    CERef<IndexBuffer>  SkyboxIndexBuffer;
    CERef<SamplerState> SkyboxSampler;

    MeshData SkyboxMesh;
};