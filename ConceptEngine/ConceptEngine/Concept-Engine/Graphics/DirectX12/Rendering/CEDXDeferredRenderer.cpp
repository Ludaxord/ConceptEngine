#include "CEDXDeferredRenderer.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../Main/RenderLayer/CESamplerState.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../Main/Common/CEMaterial.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<bool> DrawTileDebug(true);

bool CEDXDeferredRenderer::Create(Main::Rendering::CEFrameResources& FrameResources) {
	INIT_CONSOLE_VARIABLE("CE.DrawTileDebug", &DrawTileDebug);

	if (!CreateGBuffer(FrameResources)) {
		return false;
	}

	{
		Main::RenderLayer::CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = Main::RenderLayer::CESamplerMode::Clamp;
		createInfo.AddressV = Main::RenderLayer::CESamplerMode::Clamp;
		createInfo.AddressW = Main::RenderLayer::CESamplerMode::Clamp;
		createInfo.Filter = Main::RenderLayer::CESamplerFilter::MinMagMipPoint;

		FrameResources.GBufferSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
		if (!FrameResources.GBufferSampler) {
			return false;
		}
	}

	Core::Containers::CEArray<uint8> shaderCode;
	{
		Core::Containers::CEArray<Main::RenderLayer::CEShaderDefine> defines = {
			{"ENABLE_PARALLAX_MAPPING", "1"},
			{"ENABLE_NORMAL_MAPPING", "1"}
		};

		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/GeometryPass.hlsl", "VSMain", &defines,
		                                     Main::RenderLayer::CEShaderStage::Vertex,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		BaseVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!BaseVertexShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		BaseVertexShader->SetName("Geometry Pass Vertex Shader");

		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/GeometryPass.hlsl", "PSMain", &defines,
		                                     Main::RenderLayer::CEShaderStage::Pixel,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		BasePixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
		if (!BasePixelShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		BasePixelShader->SetName("Geometry Pass Pixel Shader");

		Main::RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateCreateInfo;
		depthStencilStateCreateInfo.DepthFunc = Main::RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateCreateInfo.DepthEnable = true;
		depthStencilStateCreateInfo.DepthWriteMask = Main::RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<Main::RenderLayer::CEDepthStencilState> geometryDepthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(
				depthStencilStateCreateInfo);
		if (!geometryDepthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		geometryDepthStencilState->SetName("Geometry Pass Depth Stencil State");

		Main::RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = Main::RenderLayer::CECullMode::Back;

		Core::Common::CERef<Main::RenderLayer::CERasterizerState> geometryRasterizerState = CastGraphicsManager()->
			CreateRasterizerState(
				rasterizerStateInfo);
		if (!geometryRasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		geometryRasterizerState->SetName("Geometry Pass Rasterizer State");

		Main::RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		blendStateInfo.independentBlendEnable = false;
		blendStateInfo.RenderTarget[0].BlendEnable = false;

		Core::Common::CERef<Main::RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
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
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PipelineState->SetName("Geometry Pass Pipeline State");
	}

	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/PrePass.hlsl",
		                                     "Main",
		                                     nullptr,
		                                     Main::RenderLayer::CEShaderStage::Vertex,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PrePassVertexShader = CastGraphicsManager()->CreateVertexShader(
			shaderCode);
		if (!PrePassVertexShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PrePassVertexShader->SetName("Pre Pass Vertex Shader");

		Main::RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = Main::RenderLayer::CEComparisonFunc::Less;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = Main::RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<Main::RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Pre Pass Depth Stencil State");

		Main::RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = Main::RenderLayer::CECullMode::Back;

		Core::Common::CERef<Main::RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);

		if (!rasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Pre Pass Rasterizer State");

		Main::RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		blendStateInfo.independentBlendEnable = false;
		blendStateInfo.RenderTarget[0].BlendEnable = false;

		Core::Common::CERef<Main::RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
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
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PrePassPipelineState->SetName("Pre Pass Pipeline State");
	}

	constexpr uint32 LUTSize = 512;
	constexpr Main::RenderLayer::CEFormat LUTFormat = Main::RenderLayer::CEFormat::R16G16_Float;

	if (!CastGraphicsManager()->UAVSupportsFormat(LUTFormat)) {
		CE_LOG_ERROR("[CEDXDeferredRenderer] : R16G16_Float is not supported for UAVs");

		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	Core::Common::CERef<Main::RenderLayer::CETexture2D> stagingTexture = CastGraphicsManager()->CreateTexture2D(
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
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	stagingTexture->SetName("Staging Integration LUT");

	FrameResources.IntegrationLUT = CastGraphicsManager()->CreateTexture2D(
		LUTFormat,
		LUTSize,
		LUTSize,
		1,
		1,
		Main::RenderLayer::TextureFlag_UAV,
		Main::RenderLayer::CEResourceState::Common,
		nullptr
	);

	if (!FrameResources.IntegrationLUT) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	FrameResources.IntegrationLUT->SetName("Integration LUT");

	Main::RenderLayer::CESamplerStateCreateInfo createInfo;
	createInfo.AddressU = Main::RenderLayer::CESamplerMode::Clamp;
	createInfo.AddressV = Main::RenderLayer::CESamplerMode::Clamp;
	createInfo.AddressW = Main::RenderLayer::CESamplerMode::Clamp;
	createInfo.Filter = Main::RenderLayer::CESamplerFilter::MinMagMipPoint;

	FrameResources.IntegrationLUTSampler = CastGraphicsManager()->CreateSamplerState(createInfo);

	if (!FrameResources.IntegrationLUTSampler) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	FrameResources.IntegrationLUTSampler->SetName("Integration LUT Sampler");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/BRDFIntegrationGen.hlsl", "Main", nullptr,
	                                     Main::RenderLayer::CEShaderStage::Compute,
	                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	Core::Common::CERef<Main::RenderLayer::CEComputeShader> computeShader = CastGraphicsManager()->
		CreateComputeShader(shaderCode);

	if (!computeShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	computeShader->SetName("BRDF Integration Compute Shader");

	Main::RenderLayer::CEComputePipelineStateCreateInfo pipelineStateCreateInfo;
	pipelineStateCreateInfo.Shader = computeShader.Get();

	Core::Common::CERef<Main::RenderLayer::CEComputePipelineState> BRDF_pipelineState = CastGraphicsManager()->
		CreateComputePipelineState(pipelineStateCreateInfo);
	if (!BRDF_pipelineState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	BRDF_pipelineState->SetName("BRD Integration Gen Pipeline State");

	Main::RenderLayer::CECommandList commandList;

	commandList.Execute(
		[&commandList, &stagingTexture, &BRDF_pipelineState, &computeShader, &FrameResources, &LUTSize] {
			commandList.TransitionTexture(stagingTexture.Get(), Main::RenderLayer::CEResourceState::Common,
			                              Main::RenderLayer::CEResourceState::UnorderedAccess);

			commandList.SetComputePipelineState(BRDF_pipelineState.Get());

			Main::RenderLayer::CEUnorderedAccessView* stagingUAV = stagingTexture->GetUnorderedAccessView();
			commandList.SetUnorderedAccessView(computeShader.Get(), stagingUAV, 0);

			constexpr uint32 threadCount = 16;
			const uint32 dispatchWidth = Math::CEMath::DivideByMultiple(LUTSize, threadCount);
			const uint32 dispatchHeight = Math::CEMath::DivideByMultiple(LUTSize, threadCount);
			commandList.Dispatch(dispatchWidth, dispatchHeight, 1);

			commandList.UnorderedAccessTextureBarrier(stagingTexture.Get());

			commandList.TransitionTexture(stagingTexture.Get(), Main::RenderLayer::CEResourceState::UnorderedAccess,
			                              Main::RenderLayer::CEResourceState::CopySource);
			commandList.TransitionTexture(FrameResources.IntegrationLUT.Get(),
			                              Main::RenderLayer::CEResourceState::Common,
			                              Main::RenderLayer::CEResourceState::CopyDest);

			commandList.CopyTexture(FrameResources.IntegrationLUT.Get(), stagingTexture.Get());

			commandList.TransitionTexture(FrameResources.IntegrationLUT.Get(),
			                              Main::RenderLayer::CEResourceState::CopyDest,
			                              Main::RenderLayer::CEResourceState::NonPixelShaderResource);
		});

	CommandListExecutor.ExecuteCommandList(commandList);

	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/DeferredLightPass.hlsl", "Main", nullptr,
		                                     Main::RenderLayer::CEShaderStage::Compute,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		TiledLightShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
		if (!TiledLightShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		TiledLightShader->SetName("Deferred Light Pass Shader");

		Main::RenderLayer::CEComputePipelineStateCreateInfo deferredLightPassCreateInfo;
		deferredLightPassCreateInfo.Shader = TiledLightShader.Get();

		TiledLightPassPSO = CastGraphicsManager()->CreateComputePipelineState(
			deferredLightPassCreateInfo);
		if (!TiledLightPassPSO) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		TiledLightPassPSO->SetName("Deferred Light Pass Pipeline State");
	}

	{
		Core::Containers::CEArray<Main::RenderLayer::CEShaderDefine> defines = {
			Main::RenderLayer::CEShaderDefine("DRAW_TILE_DEBUG", "1")
		};

		if (!ShaderCompiler->CompileFromFile("DirectX/Shaders/DeferredLightPass.hlsl", "Main", &defines,
		                                     Main::RenderLayer::CEShaderStage::Compute,
		                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		TiledLightDebugShader->SetName("Deferred Light Pass Debug Shader");

		Main::RenderLayer::CEComputePipelineStateCreateInfo deferredLightPassCreateInfo;
		deferredLightPassCreateInfo.Shader = TiledLightShader.Get();

		TiledLightPassPSODebug = CastGraphicsManager()->CreateComputePipelineState(
			deferredLightPassCreateInfo);
		if (!TiledLightPassPSODebug) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		TiledLightPassPSODebug->SetName("Deferred Light Pass Pipeline State Debug");
	}

	return true;
}

void CEDXDeferredRenderer::RenderPrePass(Main::RenderLayer::CECommandList& commandList,
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

void CEDXDeferredRenderer::RenderBasePass(Main::RenderLayer::CECommandList& commandList,
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

void CEDXDeferredRenderer::RenderDeferredTiledLightPass(Main::RenderLayer::CECommandList& commandList,
                                                        const Main::Rendering::CEFrameResources& frameResources,
                                                        const Main::Rendering::CELightSetup& lightSetup) {
}

bool CEDXDeferredRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
	return false;
}
