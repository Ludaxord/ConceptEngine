#include "CEDXDeferredRenderer.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../Main/RenderLayer/CESamplerState.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../Main/Common/CEMaterial.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<bool> DrawTileDebug(true);

bool CEDXDeferredRenderer::Create(Main::Rendering::CEFrameResources& FrameResources) {
	INIT_CONSOLE_VARIABLE("CE.DrawTileDebug", &DrawTileDebug);

	if (!CreateGBuffer(FrameResources)) {
		return false;
	}

	{
		CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = CESamplerMode::Clamp;
		createInfo.AddressV = CESamplerMode::Clamp;
		createInfo.AddressW = CESamplerMode::Clamp;
		createInfo.Filter = CESamplerFilter::MinMagMipPoint;

		FrameResources.GBufferSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
		if (!FrameResources.GBufferSampler) {
			return false;
		}
	}

	CEArray<uint8> shaderCode;
	{
		CEArray<CEShaderDefine> defines = {
			{"ENABLE_PARALLAX_MAPPING", "1"},
			{"ENABLE_NORMAL_MAPPING", "1"}
		};

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/GeometryPass.hlsl"),
		                                     "VSMain", &defines,
		                                     Main::RenderLayer::CEShaderStage::Vertex,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		BaseVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!BaseVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}

		BaseVertexShader->SetName("Geometry Pass Vertex Shader");

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/GeometryPass.hlsl"),
		                                     "PSMain", &defines,
		                                     Main::RenderLayer::CEShaderStage::Pixel,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		BasePixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
		if (!BasePixelShader) {
			CEDebug::DebugBreak();
			return false;
		}

		BasePixelShader->SetName("Geometry Pass Pixel Shader");

		Main::RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateCreateInfo;
		depthStencilStateCreateInfo.DepthFunc = Main::RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateCreateInfo.DepthEnable = true;
		depthStencilStateCreateInfo.DepthWriteMask = Main::RenderLayer::CEDepthWriteMask::All;

		CERef<Main::RenderLayer::CEDepthStencilState> geometryDepthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(
				depthStencilStateCreateInfo);
		if (!geometryDepthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}

		geometryDepthStencilState->SetName("Geometry Pass Depth Stencil State");

		Main::RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = Main::RenderLayer::CECullMode::Back;

		CERef<Main::RenderLayer::CERasterizerState> geometryRasterizerState = CastGraphicsManager()->
			CreateRasterizerState(
				rasterizerStateInfo);
		if (!geometryRasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}

		geometryRasterizerState->SetName("Geometry Pass Rasterizer State");

		Main::RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		blendStateInfo.independentBlendEnable = false;
		blendStateInfo.RenderTarget[0].BlendEnable = false;

		CERef<Main::RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Geometry Pass Blend State");

		Main::RenderLayer::CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
		pipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		pipelineStateInfo.BlendState = blendState.Get();
		pipelineStateInfo.DepthStencilState = geometryDepthStencilState.Get();
		pipelineStateInfo.RasterizerState = geometryRasterizerState.Get();
		pipelineStateInfo.ShaderState.VertexShader = BaseVertexShader.Get();
		pipelineStateInfo.ShaderState.PixelShader = BasePixelShader.Get();
		pipelineStateInfo.PipelineFormats.DepthStencilFormat = FrameResources.DepthBufferFormat;
		pipelineStateInfo.PipelineFormats.RenderTargetFormats[0] = Main::RenderLayer::CEFormat::R8G8B8A8_Unorm;
		pipelineStateInfo.PipelineFormats.RenderTargetFormats[1] = FrameResources.NormalFormat;
		pipelineStateInfo.PipelineFormats.RenderTargetFormats[2] = Main::RenderLayer::CEFormat::R8G8B8A8_Unorm;
		pipelineStateInfo.PipelineFormats.RenderTargetFormats[3] = FrameResources.ViewNormalFormat;
		pipelineStateInfo.PipelineFormats.NumRenderTargets = 4;

		PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(
			pipelineStateInfo);

		if (!PipelineState) {
			CEDebug::DebugBreak();
			return false;
		}

		PipelineState->SetName("Geometry Pass Pipeline State");
	}

	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/PrePass.hlsl"),
		                                     "Main",
		                                     nullptr,
		                                     Main::RenderLayer::CEShaderStage::Vertex,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PrePassVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!PrePassVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}

		PrePassVertexShader->SetName("Pre Pass Vertex Shader");

		Main::RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = Main::RenderLayer::CEComparisonFunc::Less;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = Main::RenderLayer::CEDepthWriteMask::All;

		CERef<Main::RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Pre Pass Depth Stencil State");

		Main::RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = Main::RenderLayer::CECullMode::Back;

		CERef<Main::RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);

		if (!rasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Pre Pass Rasterizer State");

		Main::RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		blendStateInfo.independentBlendEnable = false;
		blendStateInfo.RenderTarget[0].BlendEnable = false;

		CERef<Main::RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Pre Pass Blend State");

		Main::RenderLayer::CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
		pipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		pipelineStateInfo.BlendState = blendState.Get();
		pipelineStateInfo.DepthStencilState = depthStencilState.Get();
		pipelineStateInfo.RasterizerState = rasterizerState.Get();
		pipelineStateInfo.ShaderState.VertexShader = PrePassVertexShader.Get();
		pipelineStateInfo.PipelineFormats.DepthStencilFormat = FrameResources.DepthBufferFormat;

		PrePassPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(pipelineStateInfo);
		if (!PrePassPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}

		PrePassPipelineState->SetName("Pre Pass Pipeline State");
	}

	constexpr uint32 LUTSize = 512;
	constexpr Main::RenderLayer::CEFormat LUTFormat = Main::RenderLayer::CEFormat::R16G16_Float;

	if (!CastGraphicsManager()->UAVSupportsFormat(LUTFormat)) {
		CE_LOG_ERROR("[CEDXDeferredRenderer] : R16G16_Float is not supported for UAVs");

		CEDebug::DebugBreak();
		return false;
	}

	CERef<Main::RenderLayer::CETexture2D> stagingTexture = CastGraphicsManager()->CreateTexture2D(
		LUTFormat,
		LUTSize,
		LUTSize,
		1,
		1,
		Main::RenderLayer::TextureFlag_UAV,
		Main::RenderLayer::CEResourceState::Common,
		nullptr
	);
	if (!stagingTexture) {
		CEDebug::DebugBreak();
		return false;
	}

	stagingTexture->SetName("Staging Integration LUT");

	FrameResources.IntegrationLUT = CastGraphicsManager()->CreateTexture2D(
		LUTFormat,
		LUTSize,
		LUTSize,
		1,
		1,
		Main::RenderLayer::TextureFlag_SRV,
		Main::RenderLayer::CEResourceState::Common,
		nullptr
	);

	if (!FrameResources.IntegrationLUT) {
		CEDebug::DebugBreak();
		return false;
	}

	FrameResources.IntegrationLUT->SetName("Integration LUT");

	CESamplerStateCreateInfo createInfo;
	createInfo.AddressU = CESamplerMode::Clamp;
	createInfo.AddressV = CESamplerMode::Clamp;
	createInfo.AddressW = CESamplerMode::Clamp;
	createInfo.Filter = CESamplerFilter::MinMagMipPoint;

	FrameResources.IntegrationLUTSampler = CastGraphicsManager()->CreateSamplerState(createInfo);

	if (!FrameResources.IntegrationLUTSampler) {
		CEDebug::DebugBreak();
		return false;
	}

	FrameResources.IntegrationLUTSampler->SetName("Integration LUT Sampler");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/BRDFIntegrationGen.hlsl"),
	                                     "Main",
	                                     nullptr,
	                                     CEShaderStage::Compute,
	                                     CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	CERef<Main::RenderLayer::CEComputeShader> computeShader = CastGraphicsManager()->
		CreateComputeShader(shaderCode);

	if (!computeShader) {
		CEDebug::DebugBreak();
		return false;
	}

	computeShader->SetName("BRDF Integration Compute Shader");

	CEComputePipelineStateCreateInfo pipelineStateCreateInfo;
	pipelineStateCreateInfo.Shader = computeShader.Get();

	CERef<Main::RenderLayer::CEComputePipelineState> BRDF_pipelineState = CastGraphicsManager()->
		CreateComputePipelineState(pipelineStateCreateInfo);
	if (!BRDF_pipelineState) {
		CEDebug::DebugBreak();
		return false;
	}

	BRDF_pipelineState->SetName("BRDF Integration Gen Pipeline State");

	CECommandList commandList;
	commandList.Begin();
	// commandList.Execute(
	// [&commandList, &stagingTexture, &BRDF_pipelineState, &computeShader, &FrameResources, &LUTSize] {
	commandList.TransitionTexture(stagingTexture.Get(), CEResourceState::Common,
	                              CEResourceState::UnorderedAccess);

	commandList.SetComputePipelineState(BRDF_pipelineState.Get());

	CEUnorderedAccessView* stagingUAV = stagingTexture->GetUnorderedAccessView();
	commandList.SetUnorderedAccessView(computeShader.Get(), stagingUAV, 0);

	constexpr uint32 threadCount = 16;
	const uint32 dispatchWidth = Math::CEMath::DivideByMultiple(LUTSize, threadCount);
	const uint32 dispatchHeight = Math::CEMath::DivideByMultiple(LUTSize, threadCount);
	commandList.Dispatch(dispatchWidth, dispatchHeight, 1);

	commandList.UnorderedAccessTextureBarrier(stagingTexture.Get());

	commandList.TransitionTexture(stagingTexture.Get(), CEResourceState::UnorderedAccess, CEResourceState::CopySource);
	commandList.TransitionTexture(FrameResources.IntegrationLUT.Get(), CEResourceState::Common,
	                              CEResourceState::CopyDest);

	commandList.CopyTexture(FrameResources.IntegrationLUT.Get(), stagingTexture.Get());

	commandList.TransitionTexture(FrameResources.IntegrationLUT.Get(), CEResourceState::CopyDest,
	                              CEResourceState::PixelShaderResource);
	// });
	commandList.End();

	CommandListExecutor.ExecuteCommandList(commandList);

	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/DeferredLightPass.hlsl"),
		                                     "Main", nullptr, CEShaderStage::Compute, CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
		if (!TiledLightShader) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightShader->SetName("Deferred Light Pass Shader");

		Main::RenderLayer::CEComputePipelineStateCreateInfo deferredLightPassCreateInfo;
		deferredLightPassCreateInfo.Shader = TiledLightShader.Get();

		TiledLightPassPSO = CastGraphicsManager()->CreateComputePipelineState(
			deferredLightPassCreateInfo);
		if (!TiledLightPassPSO) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightPassPSO->SetName("Deferred Light Pass Pipeline State");
	}

	{
		CEArray<Main::RenderLayer::CEShaderDefine> defines = {
			Main::RenderLayer::CEShaderDefine("DRAW_TILE_DEBUG", "1")
		};

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/DeferredLightPass.hlsl"),
		                                     "Main", &defines,
		                                     Main::RenderLayer::CEShaderStage::Compute,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightDebugShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
		if (!TiledLightDebugShader) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightDebugShader->SetName("Deferred Light Pass Debug Shader");

		Main::RenderLayer::CEComputePipelineStateCreateInfo deferredLightPassCreateInfo;
		deferredLightPassCreateInfo.Shader = TiledLightDebugShader.Get();

		TiledLightPassPSODebug = CastGraphicsManager()->CreateComputePipelineState(
			deferredLightPassCreateInfo);
		if (!TiledLightPassPSODebug) {
			CEDebug::DebugBreak();
			return false;
		}

		TiledLightPassPSODebug->SetName("Deferred Light Pass Pipeline State Debug");
	}

	return true;
}

void CEDXDeferredRenderer::RenderPrePass(CECommandList& commandList,
                                         const Main::Rendering::CEFrameResources& frameResources) {
	const float renderWidth = float(frameResources.MainWindowViewport->GetWidth());
	const float renderHeight = float(frameResources.MainWindowViewport->GetHeight());

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN PRE PASS ==");

	TRACE_SCOPE("== PRE PASS ==");

	commandList.SetViewport(renderWidth, renderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
	commandList.SetScissorRect(renderWidth, renderHeight, 0, 0);

	struct PreObject {
		DirectX::XMFLOAT4X4 Matrix;
	} PerObjectBuffer;

	commandList.SetRenderTargets(nullptr, 0, frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());
	commandList.SetGraphicsPipelineState(PrePassPipelineState.Get());
	commandList.SetConstantBuffer(PrePassVertexShader.Get(), frameResources.CameraBuffer.Get(), 0);

	for (const Main::Rendering::CEMeshDrawCommand& command : frameResources.DeferredVisibleCommands) {
		if (!command.Material->HasHeightMap()) {
			commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
			commandList.SetIndexBuffer(command.IndexBuffer);

			PerObjectBuffer.Matrix = command.CurrentActor->GetTransform().GetMatrix().Native;

			commandList.Set32BitShaderConstants(PrePassVertexShader.Get(), &PerObjectBuffer, 16);
			commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
		}
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END PRE PASS ==");
}

void CEDXDeferredRenderer::RenderBasePass(CECommandList& commandList,
                                          const Main::Rendering::CEFrameResources& frameResources) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN GEOMETRY PASS ==");

	TRACE_SCOPE("== GEOMETRY PASS ==");

	const float renderWidth = float(frameResources.MainWindowViewport->GetWidth());
	const float renderHeight = float(frameResources.MainWindowViewport->GetHeight());

	commandList.SetViewport(renderWidth, renderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
	commandList.SetScissorRect(renderWidth, renderHeight, 0, 0);

	CERenderTargetView* renderTargets[] = {
		frameResources.GBuffer[BUFFER_ALBEDO_INDEX]->GetRenderTargetView(),
		frameResources.GBuffer[BUFFER_NORMAL_INDEX]->GetRenderTargetView(),
		frameResources.GBuffer[BUFFER_MATERIAL_INDEX]->GetRenderTargetView(),
		frameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetRenderTargetView()
	};
	commandList.SetRenderTargets(renderTargets, 4, frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

	commandList.SetGraphicsPipelineState(PipelineState.Get());

	struct TransformBuffer {
		DirectX::XMFLOAT4X4 Transform;
		DirectX::XMFLOAT4X4 TransformInverse;
	} TransformPerObject;

	for (const Main::Rendering::CEMeshDrawCommand& command : frameResources.DeferredVisibleCommands) {
		commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
		commandList.SetIndexBuffer(command.IndexBuffer);

		if (command.Material->IsBufferDirty()) {
			command.Material->BuildBuffer(commandList);
		}

		commandList.SetConstantBuffer(BaseVertexShader.Get(), frameResources.CameraBuffer.Get(), 0);

		CEConstantBuffer* materialBuffer = command.Material->GetMaterialBuffer();
		commandList.SetConstantBuffer(BasePixelShader.Get(), materialBuffer, 0);

		TransformPerObject.Transform = command.CurrentActor->GetTransform().GetMatrix().Native;
		TransformPerObject.TransformInverse = command.CurrentActor->GetTransform().GetMatrixInverse().Native;

		CEShaderResourceView* const* shaderResourceViews = command.Material->GetShaderResourceViews();
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[0], 0);
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[1], 1);
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[2], 2);
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[3], 3);
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[4], 4);
		commandList.SetShaderResourceView(BasePixelShader.Get(), shaderResourceViews[5], 5);

		CESamplerState* sampler = command.Material->GetMaterialSampler();
		commandList.SetSamplerState(BasePixelShader.Get(), sampler, 0);

		commandList.Set32BitShaderConstants(BaseVertexShader.Get(), &TransformPerObject, 32);

		commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END GEOMETRY PASS ==");
}

void CEDXDeferredRenderer::RenderDeferredTiledLightPass(CECommandList& commandList,
                                                        const Main::Rendering::CEFrameResources& frameResources,
                                                        const Main::Rendering::CELightSetup& lightSetup) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN LIGHT PASS ==");

	TRACE_SCOPE("== LIGHT PASS ==");

	CEComputeShader* lightPassShader = nullptr;
	if (DrawTileDebug.GetBool()) {
		lightPassShader = TiledLightDebugShader.Get();
		commandList.SetComputePipelineState(TiledLightPassPSODebug.Get());
	}
	else {
		lightPassShader = TiledLightShader.Get();
		commandList.SetComputePipelineState(TiledLightPassPSO.Get());
	}

	commandList.SetShaderResourceView(lightPassShader,
	                                  frameResources.GBuffer[BUFFER_ALBEDO_INDEX]->GetShaderResourceView(), 0);
	commandList.SetShaderResourceView(lightPassShader,
	                                  frameResources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView(), 1);
	commandList.SetShaderResourceView(lightPassShader,
	                                  frameResources.GBuffer[BUFFER_MATERIAL_INDEX]->GetShaderResourceView(), 2);
	commandList.SetShaderResourceView(lightPassShader,
	                                  frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView(), 3);

	commandList.SetShaderResourceView(lightPassShader, lightSetup.IrradianceMap->GetShaderResourceView(), 4);
	commandList.SetShaderResourceView(lightPassShader, lightSetup.SpecularIrradianceMap->GetShaderResourceView(), 5);
	commandList.SetShaderResourceView(lightPassShader, frameResources.IntegrationLUT->GetShaderResourceView(), 6);
	commandList.SetShaderResourceView(lightPassShader, lightSetup.DirLightShadowMaps->GetShaderResourceView(), 7);
	commandList.SetShaderResourceView(lightPassShader, lightSetup.PointLightShadowMaps->GetShaderResourceView(), 8);
	commandList.SetShaderResourceView(lightPassShader, frameResources.SSAOBuffer->GetShaderResourceView(), 9);

	commandList.SetConstantBuffer(lightPassShader, frameResources.CameraBuffer.Get(), 0);
	commandList.SetConstantBuffer(lightPassShader, lightSetup.PointLightsBuffer.Get(), 1);
	commandList.SetConstantBuffer(lightPassShader, lightSetup.PointLightsPosRadBuffer.Get(), 2);
	commandList.SetConstantBuffer(lightPassShader, lightSetup.ShadowCastingPointLightsBuffer.Get(), 3);
	commandList.SetConstantBuffer(lightPassShader, lightSetup.ShadowCastingPointLightsPosRadBuffer.Get(), 4);
	commandList.SetConstantBuffer(lightPassShader, lightSetup.DirectionalLightsBuffer.Get(), 5);

	commandList.SetSamplerState(lightPassShader, frameResources.IntegrationLUTSampler.Get(), 0);
	commandList.SetSamplerState(lightPassShader, frameResources.IrradianceSampler.Get(), 1);
	commandList.SetSamplerState(lightPassShader, frameResources.PointShadowSampler.Get(), 2);
	commandList.SetSamplerState(lightPassShader, frameResources.DirectionalShadowSampler.Get(), 3);

	CEUnorderedAccessView* finalTargetUAV = frameResources.FinalTarget->GetUnorderedAccessView();
	commandList.SetUnorderedAccessView(lightPassShader, finalTargetUAV, 0);

	struct LightPassSettings {
		int32 NumPointLights;
		int32 NumShadowCastingPointLights;
		int32 NumSkyLightMips;
		int32 ScreenWidth;
		int32 ScreenHeight;
	} Settings;

	Settings.NumShadowCastingPointLights = lightSetup.ShadowCastingPointLightsData.Size();
	Settings.NumPointLights = lightSetup.PointLightsData.Size();
	Settings.NumSkyLightMips = lightSetup.SpecularIrradianceMap->GetNumMips();
	Settings.ScreenWidth = frameResources.FinalTarget->GetWidth();
	Settings.ScreenHeight = frameResources.FinalTarget->GetHeight();

	commandList.Set32BitShaderConstants(lightPassShader, &Settings, 5);

	constexpr uint32 threadCount = 16;
	const uint32 workGroupWidth = Math::CEMath::DivideByMultiple<uint32>(Settings.ScreenWidth, threadCount);
	const uint32 workGroupHeight = Math::CEMath::DivideByMultiple<uint32>(Settings.ScreenHeight, threadCount);
	commandList.Dispatch(workGroupWidth, workGroupHeight, 1);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END LIGHT PASS ==");
}

bool CEDXDeferredRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
	return CreateGBuffer(resources);
}

bool CEDXDeferredRenderer::CreateGBuffer(Main::Rendering::CEFrameResources& frameResources) {
	const uint32 width = frameResources.MainWindowViewport->GetWidth();
	const uint32 height = frameResources.MainWindowViewport->GetHeight();
	const uint32 usage = TextureFlags_RenderTarget;

	//Albedo
	frameResources.GBuffer[BUFFER_ALBEDO_INDEX] = CastGraphicsManager()->CreateTexture2D(
		frameResources.AlbedoFormat, width, height, 1, 1, usage, CEResourceState::Common, nullptr);
	if (!frameResources.GBuffer[BUFFER_ALBEDO_INDEX]) {
		return false;
	}

	frameResources.GBuffer[BUFFER_ALBEDO_INDEX]->SetName("Buffer Albedo");

	//Normal
	frameResources.GBuffer[BUFFER_NORMAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
		frameResources.NormalFormat, width, height, 1, 1, usage, CEResourceState::Common, nullptr);
	if (!frameResources.GBuffer[BUFFER_NORMAL_INDEX]) {
		return false;
	}

	frameResources.GBuffer[BUFFER_NORMAL_INDEX]->SetName("Buffer Normal");

	//Material
	frameResources.GBuffer[BUFFER_MATERIAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
		frameResources.MaterialFormat, width, height, 1, 1, usage, CEResourceState::Common, nullptr);
	if (!frameResources.GBuffer[BUFFER_MATERIAL_INDEX]) {
		return false;
	}

	frameResources.GBuffer[BUFFER_MATERIAL_INDEX]->SetName("Buffer Material");

	//DepthStencil
	const uint32 usageDS = TextureFlag_DSV | TextureFlag_SRV;
	frameResources.GBuffer[BUFFER_DEPTH_INDEX] = CastGraphicsManager()->CreateTexture2D(
		frameResources.DepthBufferFormat, width, height, 1, 1, usageDS, CEResourceState::Common, nullptr);
	if (!frameResources.GBuffer[BUFFER_DEPTH_INDEX]) {
		return false;
	}

	frameResources.GBuffer[BUFFER_DEPTH_INDEX]->SetName("Buffer DepthStencil");

	//View Normal
	frameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX] = CastGraphicsManager()->CreateTexture2D(
		frameResources.ViewNormalFormat, width, height, 1, 1, usage, CEResourceState::Common, nullptr);
	if (!frameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]) {
		return false;
	}

	frameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->SetName("Buffer View Normal");

	//Final Image
	frameResources.FinalTarget = CastGraphicsManager()->CreateTexture2D(
		frameResources.FinalTargetFormat, width, height, 1, 1, usage | TextureFlag_UAV, CEResourceState::Common,
		nullptr);
	if (!frameResources.FinalTarget) {
		return false;
	}

	frameResources.FinalTarget->SetName("Final Target");

	return true;
}
