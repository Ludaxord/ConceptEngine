#include "CEDXBaseDeferredRenderer.h"

#include "../../../../Core/Application/CECore.h"

#include "../../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../Main/RenderLayer/CESamplerState.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"
#include "../../../Main/Common/CEMaterial.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<bool> DrawTileDebug(false);

bool CEDXBaseDeferredRenderer::Create(Main::Rendering::CEFrameResources& FrameResources) {
	INIT_CONSOLE_VARIABLE("CE.DrawTileDebug", &DrawTileDebug);

	if (!CreateGBuffer(FrameResources)) {
		return false;
	}

	{
		CESamplerStateCreateInfo CreateInfo;
		CreateInfo.AddressU = CESamplerMode::Clamp;
		CreateInfo.AddressV = CESamplerMode::Clamp;
		CreateInfo.AddressW = CESamplerMode::Clamp;
		CreateInfo.Filter = CESamplerFilter::MinMagMipPoint;

		FrameResources.GBufferSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
		if (!FrameResources.GBufferSampler) {
			return false;
		}
	}

	CEArray<uint8> ShaderCode;
	{
		CEArray<CEShaderDefine> Defines =
		{
			{"ENABLE_PARALLAX_MAPPING", "1"},
			{"ENABLE_NORMAL_MAPPING", "1"},
		};

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/GeometryPass.hlsl"), "VSMain", &Defines,
		                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		BaseVertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
		if (!BaseVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BaseVertexShader->SetName("GeometryPass VertexShader");
		}

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/GeometryPass.hlsl"), "PSMain", &Defines,
		                                     CEShaderStage::Pixel, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		BasePixelShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
		if (!BasePixelShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BasePixelShader->SetName("GeometryPass PixelShader");
		}

		CEDepthStencilStateCreateInfo DepthStencilStateInfo;
		DepthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
		DepthStencilStateInfo.DepthEnable = true;
		DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

		CERef<CEDepthStencilState> GeometryDepthStencilState = CastGraphicsManager()->CreateDepthStencilState(
			DepthStencilStateInfo);
		if (!GeometryDepthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			GeometryDepthStencilState->SetName("GeometryPass DepthStencilState");
		}

		CERasterizerStateCreateInfo RasterizerStateInfo;
		RasterizerStateInfo.CullMode = CECullMode::Back;

		CERef<CERasterizerState> GeometryRasterizerState = CastGraphicsManager()->CreateRasterizerState(
			RasterizerStateInfo);
		if (!GeometryRasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			GeometryRasterizerState->SetName("GeometryPass RasterizerState");
		}

		CEBlendStateCreateInfo BlendStateInfo;
		BlendStateInfo.IndependentBlendEnable = false;
		BlendStateInfo.RenderTarget[0].BlendEnable = false;

		CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
		if (!BlendState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BlendState->SetName("GeometryPass BlendState");
		}

		CEGraphicsPipelineStateCreateInfo PipelineStateInfo;
		PipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		PipelineStateInfo.BlendState = BlendState.Get();
		PipelineStateInfo.DepthStencilState = GeometryDepthStencilState.Get();
		PipelineStateInfo.RasterizerState = GeometryRasterizerState.Get();
		PipelineStateInfo.ShaderState.VertexShader = BaseVertexShader.Get();
		PipelineStateInfo.ShaderState.PixelShader = BasePixelShader.Get();
		PipelineStateInfo.PipelineFormats.DepthStencilFormat = FrameResources.DepthBufferFormat;
		PipelineStateInfo.PipelineFormats.RenderTargetFormats[0] = CEFormat::R8G8B8A8_Unorm;
		PipelineStateInfo.PipelineFormats.RenderTargetFormats[1] = FrameResources.NormalFormat;
		PipelineStateInfo.PipelineFormats.RenderTargetFormats[2] = CEFormat::R8G8B8A8_Unorm;
		PipelineStateInfo.PipelineFormats.RenderTargetFormats[3] = FrameResources.ViewNormalFormat;
		PipelineStateInfo.PipelineFormats.NumRenderTargets = 4;

		PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PipelineStateInfo);
		if (!PipelineState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PipelineState->SetName("GeometryPass PipelineState");
		}
	}

	// PrePass
	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/PrePass.hlsl"), "Main", nullptr,
		                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PrePassVertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
		if (!PrePassVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PrePassVertexShader->SetName("PrePass VertexShader");
		}

		CEDepthStencilStateCreateInfo DepthStencilStateInfo;
		DepthStencilStateInfo.DepthFunc = CEComparisonFunc::Less;
		DepthStencilStateInfo.DepthEnable = true;
		DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

		CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->CreateDepthStencilState(DepthStencilStateInfo);
		if (!DepthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			DepthStencilState->SetName("Prepass DepthStencilState");
		}

		CERasterizerStateCreateInfo RasterizerStateInfo;
		RasterizerStateInfo.CullMode = CECullMode::Back;

		CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
		if (!RasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			RasterizerState->SetName("Prepass RasterizerState");
		}

		CEBlendStateCreateInfo BlendStateInfo;
		BlendStateInfo.IndependentBlendEnable = false;
		BlendStateInfo.RenderTarget[0].BlendEnable = false;

		CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
		if (!BlendState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BlendState->SetName("Prepass BlendState");
		}

		CEGraphicsPipelineStateCreateInfo PipelineStateInfo;
		PipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		PipelineStateInfo.BlendState = BlendState.Get();
		PipelineStateInfo.DepthStencilState = DepthStencilState.Get();
		PipelineStateInfo.RasterizerState = RasterizerState.Get();
		PipelineStateInfo.ShaderState.VertexShader = PrePassVertexShader.Get();
		PipelineStateInfo.PipelineFormats.DepthStencilFormat = FrameResources.DepthBufferFormat;

		PrePassPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PipelineStateInfo);
		if (!PrePassPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PrePassPipelineState->SetName("PrePass PipelineState");
		}
	}

	constexpr uint32 LUTSize = 512;
	constexpr CEFormat LUTFormat = CEFormat::R16G16_Float;
	if (!CastGraphicsManager()->UAVSupportsFormat(LUTFormat)) {
		CE_LOG_ERROR("[Renderer]: R16G16_Float is not supported for UAVs");

		CEDebug::DebugBreak();
		return false;
	}

	CERef<CETexture2D> StagingTexture = CastGraphicsManager()->CreateTexture2D(LUTFormat, LUTSize, LUTSize, 1, 1, TextureFlag_UAV,
	                                                 CEResourceState::Common, nullptr);
	if (!StagingTexture) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		StagingTexture->SetName("Staging IntegrationLUT");
	}

	FrameResources.IntegrationLUT = CastGraphicsManager()->CreateTexture2D(LUTFormat, LUTSize, LUTSize, 1, 1, TextureFlag_SRV,
	                                                CEResourceState::Common, nullptr);
	if (!FrameResources.IntegrationLUT) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FrameResources.IntegrationLUT->SetName("IntegrationLUT");
	}

	CESamplerStateCreateInfo CreateInfo;
	CreateInfo.AddressU = CESamplerMode::Clamp;
	CreateInfo.AddressV = CESamplerMode::Clamp;
	CreateInfo.AddressW = CESamplerMode::Clamp;
	CreateInfo.Filter = CESamplerFilter::MinMagMipPoint;

	FrameResources.IntegrationLUTSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
	if (!FrameResources.IntegrationLUTSampler) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FrameResources.IntegrationLUTSampler->SetName("IntegrationLUT Sampler");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/BRDFIntegrationGen.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	CERef<CEComputeShader> CShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
	if (!CShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		CShader->SetName("BRDFIntegationGen ComputeShader");
	}

	CEComputePipelineStateCreateInfo PipelineStateInfo;
	PipelineStateInfo.Shader = CShader.Get();

	CERef<CEComputePipelineState> BRDF_PipelineState = CastGraphicsManager()->CreateComputePipelineState(PipelineStateInfo);
	if (!BRDF_PipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BRDF_PipelineState->SetName("BRDFIntegationGen PipelineState");
	}

	CECommandList CommandList;
	CommandList.Begin();

	CommandList.TransitionTexture(StagingTexture.Get(), CEResourceState::Common, CEResourceState::UnorderedAccess);

	CommandList.SetComputePipelineState(BRDF_PipelineState.Get());

	CEUnorderedAccessView* StagingUAV = StagingTexture->GetUnorderedAccessView();
	CommandList.SetUnorderedAccessView(CShader.Get(), StagingUAV, 0);

	constexpr uint32 ThreadCount = 16;
	const uint32 DispatchWidth = Math::CEMath::DivideByMultiple(LUTSize, ThreadCount);
	const uint32 DispatchHeight = Math::CEMath::DivideByMultiple(LUTSize, ThreadCount);
	CommandList.Dispatch(DispatchWidth, DispatchHeight, 1);

	CommandList.UnorderedAccessTextureBarrier(StagingTexture.Get());

	CommandList.TransitionTexture(StagingTexture.Get(), CEResourceState::UnorderedAccess, CEResourceState::CopySource);
	CommandList.TransitionTexture(FrameResources.IntegrationLUT.Get(), CEResourceState::Common, CEResourceState::CopyDest);

	CommandList.CopyTexture(FrameResources.IntegrationLUT.Get(), StagingTexture.Get());

	CommandList.TransitionTexture(FrameResources.IntegrationLUT.Get(), CEResourceState::CopyDest,
	                          CEResourceState::PixelShaderResource);

	CommandList.End();
	CommandListExecutor.ExecuteCommandList(CommandList);

	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/DeferredLightPass.hlsl"), "Main", nullptr,
		                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
		if (!TiledLightShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			TiledLightShader->SetName("DeferredLightPass Shader");
		}

		CEComputePipelineStateCreateInfo DeferredLightPassCreateInfo;
		DeferredLightPassCreateInfo.Shader = TiledLightShader.Get();

		TiledLightPassPSO = CastGraphicsManager()->CreateComputePipelineState(DeferredLightPassCreateInfo);
		if (!TiledLightPassPSO) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			TiledLightPassPSO->SetName("DeferredLightPass PipelineState");
		}
	}

	{
		CEArray<CEShaderDefine> Defines =
		{
			CEShaderDefine("DRAW_TILE_DEBUG", "1")
		};

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/DeferredLightPass.hlsl"), "Main", &Defines,
		                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightDebugShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
		if (!TiledLightDebugShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			TiledLightDebugShader->SetName("DeferredLightPass Debug Shader");
		}

		CEComputePipelineStateCreateInfo DeferredLightPassCreateInfo;
		DeferredLightPassCreateInfo.Shader = TiledLightDebugShader.Get();

		TiledLightPassPSODebug = CastGraphicsManager()->CreateComputePipelineState(DeferredLightPassCreateInfo);
		if (!TiledLightPassPSODebug) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			TiledLightPassPSODebug->SetName("DeferredLightPass PipelineState Debug");
		}
	}

	return true;

}

void CEDXBaseDeferredRenderer::RenderPrePass(CECommandList& CommandList,
                                             const Main::Rendering::CEFrameResources& FrameResources) {
	const float RenderWidth  = float(FrameResources.MainWindowViewport->GetWidth());
    const float RenderHeight = float(FrameResources.MainWindowViewport->GetHeight());

    INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin PrePass");

    TRACE_SCOPE("PrePass");

    CommandList.SetViewport(RenderWidth, RenderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
    CommandList.SetScissorRect(RenderWidth, RenderHeight, 0, 0);

    struct PerObject
    {
        XMFLOAT4X4 Matrix;
    } PerObjectBuffer;

    CommandList.SetRenderTargets(nullptr, 0, FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

    CommandList.SetGraphicsPipelineState(PrePassPipelineState.Get());

    CommandList.SetConstantBuffer(PrePassVertexShader.Get(), FrameResources.CameraBuffer.Get(), 0);

    for (const Main::Rendering::CEMeshDrawCommand& Command : FrameResources.DeferredVisibleCommands)
    {
        if (!Command.Material->HasHeightMap())
        {
            CommandList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
            CommandList.SetIndexBuffer(Command.IndexBuffer);

            PerObjectBuffer.Matrix = Command.CurrentActor->GetTransform().GetMatrix().Native;

            CommandList.Set32BitShaderConstants(PrePassVertexShader.Get(), &PerObjectBuffer, 16);

            CommandList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
        }
    }

    INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End PrePass");
}

void CEDXBaseDeferredRenderer::RenderBasePass(CECommandList& CommandList,
                                              const Main::Rendering::CEFrameResources& FrameResources) {
	 INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin GeometryPass");

    TRACE_SCOPE("GeometryPass");

    const float RenderWidth  = float(FrameResources.MainWindowViewport->GetWidth());
    const float RenderHeight = float(FrameResources.MainWindowViewport->GetHeight());

    CommandList.SetViewport(RenderWidth, RenderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
    CommandList.SetScissorRect(RenderWidth, RenderHeight, 0, 0);

    CERenderTargetView* RenderTargets[] =
    {
        FrameResources.GBuffer[BUFFER_ALBEDO_INDEX]->GetRenderTargetView(),
        FrameResources.GBuffer[BUFFER_NORMAL_INDEX]->GetRenderTargetView(),
        FrameResources.GBuffer[BUFFER_MATERIAL_INDEX]->GetRenderTargetView(),
        FrameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetRenderTargetView(),
    };
    CommandList.SetRenderTargets(RenderTargets, 4, FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

    // Setup Pipeline
    CommandList.SetGraphicsPipelineState(PipelineState.Get());

    struct TransformBuffer
    {
        XMFLOAT4X4 Transform;
        XMFLOAT4X4 TransformInv;
    } TransformPerObject;

    for (const Main::Rendering::CEMeshDrawCommand& Command : FrameResources.DeferredVisibleCommands)
    {
        CommandList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
        CommandList.SetIndexBuffer(Command.IndexBuffer);

        if (Command.Material->IsBufferDirty())
        {
            Command.Material->BuildBuffer(CommandList);
        }

        CommandList.SetConstantBuffer(BaseVertexShader.Get(), FrameResources.CameraBuffer.Get(), 0);

        CEConstantBuffer* MaterialBuffer = Command.Material->GetMaterialBuffer();
        CommandList.SetConstantBuffer(BasePixelShader.Get(), MaterialBuffer, 0);

        TransformPerObject.Transform    = Command.CurrentActor->GetTransform().GetMatrix().Native;
        TransformPerObject.TransformInv = Command.CurrentActor->GetTransform().GetMatrixInverse().Native;

        CEShaderResourceView* const* ShaderResourceViews = Command.Material->GetShaderResourceViews();
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[0], 0);
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[1], 1);
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[2], 2);
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[3], 3);
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[4], 4);
        CommandList.SetShaderResourceView(BasePixelShader.Get(), ShaderResourceViews[5], 5);

        CESamplerState* Sampler = Command.Material->GetMaterialSampler();
        CommandList.SetSamplerState(BasePixelShader.Get(), Sampler, 0);

        CommandList.Set32BitShaderConstants(BaseVertexShader.Get(), &TransformPerObject, 32);

        CommandList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
    }

    INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End GeometryPass");
}

void CEDXBaseDeferredRenderer::RenderDeferredTiledLightPass(CECommandList& commandList,
                                                            const Main::Rendering::CEFrameResources& frameResources,
                                                            const Main::Rendering::CELightSetup& lightSetup) {
}

bool CEDXBaseDeferredRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
    return CreateGBuffer(resources);
}

bool CEDXBaseDeferredRenderer::CreateGBuffer(Main::Rendering::CEFrameResources& FrameResources) {
const uint32 Width  = FrameResources.MainWindowViewport->GetWidth();
    const uint32 Height = FrameResources.MainWindowViewport->GetHeight();
    const uint32 Usage  = TextureFlags_RenderTarget;

    // Albedo
    FrameResources.GBuffer[BUFFER_ALBEDO_INDEX] =CastGraphicsManager()->CreateTexture2D(
        FrameResources.AlbedoFormat, 
        Width, Height, 1, 1, Usage, 
        CEResourceState::Common, 
        nullptr);
    if (FrameResources.GBuffer[BUFFER_ALBEDO_INDEX])
    {
        FrameResources.GBuffer[BUFFER_ALBEDO_INDEX]->SetName("GBuffer Albedo");
    }
    else
    {
        return false;
    }

    // Normal
    FrameResources.GBuffer[BUFFER_NORMAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
        FrameResources.NormalFormat, 
        Width, Height, 1, 1, Usage, 
        CEResourceState::Common, 
        nullptr);
    if (FrameResources.GBuffer[BUFFER_NORMAL_INDEX])
    {
        FrameResources.GBuffer[BUFFER_NORMAL_INDEX]->SetName("GBuffer Normal");
    }
    else
    {
        return false;
    }

    // Material Properties
    FrameResources.GBuffer[BUFFER_MATERIAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
        FrameResources.MaterialFormat, 
        Width, Height, 1, 1, Usage, 
        CEResourceState::Common,
        nullptr);
    if (FrameResources.GBuffer[BUFFER_MATERIAL_INDEX])
    {
        FrameResources.GBuffer[BUFFER_MATERIAL_INDEX]->SetName("GBuffer Material");
    }
    else
    {
        return false;
    }

    // DepthStencil
    const uint32 UsageDS = TextureFlag_DSV | TextureFlag_SRV;
    FrameResources.GBuffer[BUFFER_DEPTH_INDEX] = CastGraphicsManager()->CreateTexture2D(
        FrameResources.DepthBufferFormat, 
        Width, Height, 1, 1, UsageDS, 
        CEResourceState::Common, 
        nullptr);
    if (FrameResources.GBuffer[BUFFER_DEPTH_INDEX])
    {
        FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->SetName("GBuffer DepthStencil");
    }
    else
    {
        return false;
    }

    // View Normal
    FrameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
        FrameResources.ViewNormalFormat, 
        Width, Height, 1, 1, Usage, 
        CEResourceState::Common, 
        nullptr);
    if (FrameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX])
    {
        FrameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->SetName("GBuffer ViewNormal");
    }
    else
    {
        return false;
    }

    // Final Image
    FrameResources.FinalTarget = CastGraphicsManager()->CreateTexture2D(
        FrameResources.FinalTargetFormat, 
        Width, Height, 1, 1, 
        Usage | TextureFlag_UAV, 
        CEResourceState::Common, 
        nullptr);
    if (FrameResources.FinalTarget)
    {
        FrameResources.FinalTarget->SetName("Final Target");
    }
    else
    {
        return false;
    }

    return true;}

void CEDXBaseDeferredRenderer::RenderDeferredTiledLightPass(Main::RenderLayer::CECommandList& CommandList,
	const Main::Rendering::CEFrameResources& FrameResources, const Main::Rendering::CEBaseLightSetup& LightSetup) {
	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin LightPass");

    TRACE_SCOPE("LightPass");

    CEComputeShader* LightPassShader = nullptr;
    if (DrawTileDebug.GetBool())
    {
        LightPassShader = TiledLightDebugShader.Get();
        CommandList.SetComputePipelineState(TiledLightPassPSODebug.Get());
    }
    else
    {
        LightPassShader = TiledLightShader.Get();
        CommandList.SetComputePipelineState(TiledLightPassPSO.Get());
    }

    CommandList.SetShaderResourceView(LightPassShader, FrameResources.GBuffer[BUFFER_ALBEDO_INDEX]->GetShaderResourceView(), 0);
    CommandList.SetShaderResourceView(LightPassShader, FrameResources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView(), 1);
    CommandList.SetShaderResourceView(LightPassShader, FrameResources.GBuffer[BUFFER_MATERIAL_INDEX]->GetShaderResourceView(), 2);
    CommandList.SetShaderResourceView(LightPassShader, FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView(), 3);
    //CommandList.SetShaderResourceView(LightPassShader, nullptr, 4); // Reflection
    CommandList.SetShaderResourceView(LightPassShader, LightSetup.IrradianceMap->GetShaderResourceView(), 4);
    CommandList.SetShaderResourceView(LightPassShader, LightSetup.SpecularIrradianceMap->GetShaderResourceView(), 5);
    CommandList.SetShaderResourceView(LightPassShader, FrameResources.IntegrationLUT->GetShaderResourceView(), 6);
    CommandList.SetShaderResourceView(LightPassShader, LightSetup.DirLightShadowMaps->GetShaderResourceView(), 7);
    CommandList.SetShaderResourceView(LightPassShader, LightSetup.PointLightShadowMaps->GetShaderResourceView(), 8);
    CommandList.SetShaderResourceView(LightPassShader, FrameResources.SSAOBuffer->GetShaderResourceView(), 9);

    CommandList.SetConstantBuffer(LightPassShader, FrameResources.CameraBuffer.Get(), 0);
    CommandList.SetConstantBuffer(LightPassShader, LightSetup.PointLightsBuffer.Get(), 1);
    CommandList.SetConstantBuffer(LightPassShader, LightSetup.PointLightsPosRadBuffer.Get(), 2);
    CommandList.SetConstantBuffer(LightPassShader, LightSetup.ShadowCastingPointLightsBuffer.Get(), 3);
    CommandList.SetConstantBuffer(LightPassShader, LightSetup.ShadowCastingPointLightsPosRadBuffer.Get(), 4);
    CommandList.SetConstantBuffer(LightPassShader, LightSetup.DirectionalLightsBuffer.Get(), 5);

    CommandList.SetSamplerState(LightPassShader, FrameResources.IntegrationLUTSampler.Get(), 0);
    CommandList.SetSamplerState(LightPassShader, FrameResources.IrradianceSampler.Get(), 1);
    CommandList.SetSamplerState(LightPassShader, FrameResources.PointShadowSampler.Get(), 2);
    CommandList.SetSamplerState(LightPassShader, FrameResources.DirectionalShadowSampler.Get(), 3);

    CEUnorderedAccessView* FinalTargetUAV = FrameResources.FinalTarget->GetUnorderedAccessView();
    CommandList.SetUnorderedAccessView(LightPassShader, FinalTargetUAV, 0);

    struct LightPassSettings
    {
        int32 NumPointLights;
        int32 NumShadowCastingPointLights;
        int32 NumSkyLightMips;
        int32 ScreenWidth;
        int32 ScreenHeight;
    } Settings;

    Settings.NumShadowCastingPointLights = LightSetup.ShadowCastingPointLightsData.Size();
    Settings.NumPointLights  = LightSetup.PointLightsData.Size();
    Settings.NumSkyLightMips = LightSetup.SpecularIrradianceMap->GetNumMips();
    Settings.ScreenWidth     = FrameResources.FinalTarget->GetWidth();
    Settings.ScreenHeight    = FrameResources.FinalTarget->GetHeight();

    CommandList.Set32BitShaderConstants(LightPassShader, &Settings, 5);

    constexpr uint32 ThreadCount = 16;
    const uint32 WorkGroupWidth  = Math::CEMath::DivideByMultiple<uint32>(Settings.ScreenWidth, ThreadCount);
    const uint32 WorkGroupHeight = Math::CEMath::DivideByMultiple<uint32>(Settings.ScreenHeight, ThreadCount);
    CommandList.Dispatch(WorkGroupWidth, WorkGroupHeight, 1);

    INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End LightPass");
}
