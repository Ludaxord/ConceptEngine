#pragma once
#include "FrameResources.h"

#include "../RenderLayer/CommandList.h"

#include "../Scene/CEScene.h"
#include "../Graphics/D3D12/Managers/CEDX12MeshManager.h"

class SkyboxRenderPass final
{
public:
    SkyboxRenderPass()  = default;
    ~SkyboxRenderPass() = default;

    bool Init(FrameResources& FrameResources);

    void Render(CommandList& CmdList, const FrameResources& FrameResources, const CEScene& Scene);

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
