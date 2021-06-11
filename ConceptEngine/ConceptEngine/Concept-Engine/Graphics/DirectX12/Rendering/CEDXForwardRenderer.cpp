#include "CEDXForwardRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../Main/Common/CEMaterial.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXForwardRenderer::Create(const Main::Rendering::CEFrameResources& resources) {

	CEArray<Main::RenderLayer::CEShaderDefine> defines = {
		{"ENABLE_PARALLAX_MAPPING", "1"},
		{"ENABLE_NORMAL_MAPPING", "1"}
	};

	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ForwardPass.hlsl",
	                                     "VSMain",
	                                     &defines,
	                                     Main::RenderLayer::CEShaderStage::Vertex,
	                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	VertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!VertexShader) {
		CEDebug::DebugBreak();
		return false;
	}

	VertexShader->SetName("Forward Pass Vertex Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ForwardPass.hlsl", "PSMain", &defines, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	PixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!PixelShader) {
		CEDebug::DebugBreak();
		return false;
	}

	PixelShader->SetName("Forward Pass Pixel Shader");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	depthStencilStateInfo.DepthEnable = true;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

	CERef<CEDepthStencilState> depthStencilState = CastGraphicsManager()->CreateDepthStencilState(
		depthStencilStateInfo);
	if (!depthStencilState) {
		CEDebug::DebugBreak();
		return false;
	}

	depthStencilState->SetName("Forward Pass Depth Stencil State");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(
		rasterizerStateInfo);
	if (!rasterizerState) {
		CEDebug::DebugBreak();
		return false;
	}

	rasterizerState->SetName("Forward Pass Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;
	blendStateInfo.independentBlendEnable = false;
	blendStateInfo.RenderTarget[0].BlendEnable = true;

	CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);
	if (!blendState) {
		CEDebug::DebugBreak();
		return false;
	}

	blendState->SetName("Forward Pass Blend State");

	CEGraphicsPipelineStateCreateInfo psoProperties;
	psoProperties.ShaderState.VertexShader = VertexShader.Get();
	psoProperties.ShaderState.PixelShader = PixelShader.Get();
	psoProperties.InputLayoutState = resources.StdInputLayout.Get();
	psoProperties.DepthStencilState = depthStencilState.Get();
	psoProperties.BlendState = blendState.Get();
	psoProperties.RasterizerState = rasterizerState.Get();
	psoProperties.PipelineFormats.RenderTargetFormats[0] = resources.FinalTargetFormat;
	psoProperties.PipelineFormats.DepthStencilFormat = resources.DepthBufferFormat;
	psoProperties.PipelineFormats.NumRenderTargets = 1;
	psoProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;

	PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(psoProperties);
	if (!PipelineState) {
		CEDebug::DebugBreak();
		return false;
	}

	PipelineState->SetName("Forward Pipeline State");

	return true;
}

void CEDXForwardRenderer::Render(Main::RenderLayer::CECommandList& commandList,
                                 const Main::Rendering::CEFrameResources& frameResources,
                                 const Main::Rendering::CELightSetup& lightSetup) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN FORWARD PASS ==");

	TRACE_SCOPE("Forward Pass");

	const float renderWidth = float(frameResources.FinalTarget->GetWidth());
	const float renderHeight = float(frameResources.FinalTarget->GetHeight());

	commandList.SetViewport(renderWidth, renderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
	commandList.SetScissorRect(renderWidth, renderHeight, 0, 0);

	CERenderTargetView* finalTargetRTV = frameResources.FinalTarget->GetRenderTargetView();
	commandList.SetRenderTargets(&finalTargetRTV, 1, frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

	commandList.SetConstantBuffer(PixelShader.Get(), frameResources.CameraBuffer.Get(), 0);

	commandList.SetConstantBuffer(PixelShader.Get(), lightSetup.ShadowCastingPointLightsBuffer.Get(), 1);
	commandList.SetConstantBuffer(PixelShader.Get(), lightSetup.ShadowCastingPointLightsPosRadBuffer.Get(), 2);
	commandList.SetConstantBuffer(PixelShader.Get(), lightSetup.DirectionalLightsBuffer.Get(), 3);

	commandList.SetShaderResourceView(PixelShader.Get(), lightSetup.IrradianceMap->GetShaderResourceView(), 0);
	commandList.SetShaderResourceView(PixelShader.Get(), lightSetup.SpecularIrradianceMap->GetShaderResourceView(), 1);
	commandList.SetShaderResourceView(PixelShader.Get(), frameResources.IntegrationLUT->GetShaderResourceView(), 2);
	commandList.SetShaderResourceView(PixelShader.Get(), lightSetup.DirLightShadowMaps->GetShaderResourceView(), 3);
	commandList.SetShaderResourceView(PixelShader.Get(), lightSetup.PointLightShadowMaps->GetShaderResourceView(), 4);

	commandList.SetSamplerState(PixelShader.Get(), frameResources.IntegrationLUTSampler.Get(), 1);
	commandList.SetSamplerState(PixelShader.Get(), frameResources.IrradianceSampler.Get(), 2);
	commandList.SetSamplerState(PixelShader.Get(), frameResources.PointShadowSampler.Get(), 3);
	commandList.SetSamplerState(PixelShader.Get(), frameResources.DirectionalShadowSampler.Get(), 4);

	struct TransformBuffer {
		DirectX::XMFLOAT4X4 Transform;
		DirectX::XMFLOAT4X4 TransformInverse;
	} TransformPerObject;

	commandList.SetGraphicsPipelineState(PipelineState.Get());
	for (const Main::Rendering::CEMeshDrawCommand& command : frameResources.ForwardVisibleCommands) {
		commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
		commandList.SetIndexBuffer(command.IndexBuffer);

		if (command.Material->IsBufferDirty()) {
			command.Material->BuildBuffer(commandList);
		}

		CEConstantBuffer* constantBuffer = command.Material->GetMaterialBuffer();
		commandList.SetConstantBuffer(PixelShader.Get(), constantBuffer, 4);

		CEShaderResourceView* const * shaderResourceViews = command.Material->GetShaderResourceViews();
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[0], 5);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[1], 6);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[2], 7);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[3], 8);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[4], 9);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[5], 10);
		commandList.SetShaderResourceView(PixelShader.Get(), shaderResourceViews[6], 11);

		CESamplerState* samplerState = command.Material->GetMaterialSampler();
		commandList.SetSamplerState(PixelShader.Get(), samplerState, 0);

		TransformPerObject.Transform = command.CurrentActor->GetTransform().GetMatrix().Native;
		TransformPerObject.TransformInverse = command.CurrentActor->GetTransform().GetMatrixInverse().Native;

		commandList.Set32BitShaderConstants(VertexShader.Get(), &TransformPerObject, 32);
		commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END FORWARD PASS ==");
}
