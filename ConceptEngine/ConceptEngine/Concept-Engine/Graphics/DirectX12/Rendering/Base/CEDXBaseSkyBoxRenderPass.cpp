#include "CEDXBaseSkyBoxRenderPass.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"
#include "../../../../Render/Scene/CEScene.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"

#include "../../../Main/Common/CETextureData.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

bool CEDXBaseSkyBoxRenderPass::Create(
	Main::Rendering::CEFrameResources& FrameResources, const Main::Rendering::CEPanoramaConfig& panoramaConfig) {
	SkyboxMesh = CastMeshManager()->CreateSphere(1);

    CEResourceData VertexData = CEResourceData(SkyboxMesh.Vertices.Data(), SkyboxMesh.Vertices.SizeInBytes());
    SkyboxVertexBuffer = CastGraphicsManager()->CreateVertexBuffer<Main::CEVertex>(SkyboxMesh.Vertices.Size(), BufferFlag_Upload, CEResourceState::VertexAndConstantBuffer, &VertexData);
    if (!SkyboxVertexBuffer)
    {
        return false;
    }
    else
    {
        SkyboxVertexBuffer->SetName("Skybox VertexBuffer");
    }

    CEResourceData IndexData = CEResourceData(SkyboxMesh.Indices.Data(), SkyboxMesh.Indices.SizeInBytes());
    SkyboxIndexBuffer = CastGraphicsManager()->CreateIndexBuffer(CEIndexFormat::uint32, SkyboxMesh.Indices.Size(), BufferFlag_Upload, CEResourceState::VertexAndConstantBuffer, &IndexData);
    if (!SkyboxIndexBuffer)
    {
        return false;
    }
    else
    {
        SkyboxIndexBuffer->SetName("Skybox IndexBuffer");
    }

    // Create Texture Cube
    const std::string PanoramaSourceFilename = "../Assets/Textures/arches.hdr";
    CERef<CETexture2D> Panorama = CastTextureManager()->LoadFromFile(PanoramaSourceFilename, 0, CEFormat::R32G32B32A32_Float);
    if (!Panorama)
    {
        return false;
    }
    else
    {
        Panorama->SetName(PanoramaSourceFilename);
    }

    FrameResources.Skybox = CastTextureManager()->CreateTextureCubeFromPanorama(Panorama.Get(), 1024, Main::Managers::TextureFlag_GenerateMips, CEFormat::R16G16B16A16_Float);
    if (!FrameResources.Skybox)
    {
        return false;
    }
    else
    {
        FrameResources.Skybox->SetName("Skybox");
    }

    CESamplerStateCreateInfo CreateInfo;
    CreateInfo.AddressU = CESamplerMode::Wrap;
    CreateInfo.AddressV = CESamplerMode::Wrap;
    CreateInfo.AddressW = CESamplerMode::Wrap;
    CreateInfo.Filter   = CESamplerFilter::MinMagMipLinear;
    CreateInfo.MinLOD   = 0.0f;
    CreateInfo.MaxLOD   = 0.0f;

    SkyboxSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
    if (!SkyboxSampler)
    {
        return false;
    }

    CEArray<uint8> ShaderCode;
    if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Skybox.hlsl"), "VSMain", nullptr, CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode))
    {
        CEDebug::DebugBreak();
        return false;
    }

    SkyboxVertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
    if (!SkyboxVertexShader)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        SkyboxVertexShader->SetName("Skybox VertexShader");
    }

    if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Skybox.hlsl"), "PSMain", nullptr, CEShaderStage::Pixel, CEShaderModel::SM_6_0, ShaderCode))
    {
        CEDebug::DebugBreak();
        return false;
    }

    SkyboxPixelShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
    if (!SkyboxPixelShader)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        SkyboxPixelShader->SetName("Skybox PixelShader");
    }

    CERasterizerStateCreateInfo RasterizerStateInfo;
    RasterizerStateInfo.CullMode = CECullMode::None;

    CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
    if (!RasterizerState)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        RasterizerState->SetName("Skybox RasterizerState");
    }

    CEBlendStateCreateInfo BlendStateInfo;
    BlendStateInfo.IndependentBlendEnable      = false;
    BlendStateInfo.RenderTarget[0].BlendEnable = false;

    CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
    if (!BlendState)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        BlendState->SetName("Skybox BlendState");
    }

    CEDepthStencilStateCreateInfo DepthStencilStateInfo;
    DepthStencilStateInfo.DepthFunc      = CEComparisonFunc::LessEqual;
    DepthStencilStateInfo.DepthEnable    = true;
    DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

    CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->CreateDepthStencilState(DepthStencilStateInfo);
    if (!DepthStencilState)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        DepthStencilState->SetName("Skybox DepthStencilState");
    }

    CEGraphicsPipelineStateCreateInfo PipelineStateInfo;
    PipelineStateInfo.InputLayoutState                       = FrameResources.StdInputLayout.Get();
    PipelineStateInfo.BlendState                             = BlendState.Get();
    PipelineStateInfo.DepthStencilState                      = DepthStencilState.Get();
    PipelineStateInfo.RasterizerState                        = RasterizerState.Get();
    PipelineStateInfo.ShaderState.VertexShader               = SkyboxVertexShader.Get();
    PipelineStateInfo.ShaderState.PixelShader                = SkyboxPixelShader.Get();
    PipelineStateInfo.PipelineFormats.RenderTargetFormats[0] = FrameResources.FinalTargetFormat;
    PipelineStateInfo.PipelineFormats.NumRenderTargets       = 1;
    PipelineStateInfo.PipelineFormats.DepthStencilFormat     = FrameResources.DepthBufferFormat;

    PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PipelineStateInfo);
    if (!PipelineState)
    {
        CEDebug::DebugBreak();
        return false;
    }
    else
    {
        PipelineState->SetName("SkyboxPSO PipelineState");
    }

    return true;
}

void CEDXBaseSkyBoxRenderPass::Render(
	Main::RenderLayer::CECommandList& CmdList, const Main::Rendering::CEFrameResources& FrameResources,
	const Render::Scene::CEScene& Scene) {
	 INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin Skybox");

    TRACE_SCOPE("Render Skybox");

    const float RenderWidth  = float(FrameResources.FinalTarget->GetWidth());
    const float RenderHeight = float(FrameResources.FinalTarget->GetHeight());

    CERenderTargetView* RenderTarget[] = { FrameResources.FinalTarget->GetRenderTargetView() };
    CmdList.SetRenderTargets(RenderTarget, 1, nullptr);

    CmdList.SetViewport(RenderWidth, RenderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
    CmdList.SetScissorRect(RenderWidth, RenderHeight, 0, 0);

    CmdList.SetRenderTargets(RenderTarget, 1, FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

    CmdList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);
    CmdList.SetVertexBuffers(&SkyboxVertexBuffer, 1, 0);
    CmdList.SetIndexBuffer(SkyboxIndexBuffer.Get());
    CmdList.SetGraphicsPipelineState(PipelineState.Get());

    struct SimpleCameraBuffer
    {
        XMFLOAT4X4 Matrix;
    } SimpleCamera;
    SimpleCamera.Matrix = Scene.GetCamera()->GetViewProjectionWithoutTranslateMatrix().Native;

    CmdList.Set32BitShaderConstants(SkyboxVertexShader.Get(), &SimpleCamera, 16);

    CEShaderResourceView* SkyboxSRV = FrameResources.Skybox->GetShaderResourceView();
    CmdList.SetShaderResourceView(SkyboxPixelShader.Get(), SkyboxSRV, 0);

    CmdList.SetSamplerState(SkyboxPixelShader.Get(), SkyboxSampler.Get(), 0);

    CmdList.DrawIndexedInstanced(static_cast<uint32>(SkyboxMesh.Indices.Size()), 1, 0, 0, 0);

    INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End Skybox");
}
