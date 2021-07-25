#include "CEDXBaseForwardRenderer.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"
#include "../../../Main/Common/CEMaterial.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"
using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

bool CEDXBaseForwardRenderer::Create(
	const Main::Rendering::CEFrameResources& FrameResources) {
	CEArray<Main::RenderLayer::CEShaderDefine> Defines =
	{
		{"ENABLE_PARALLAX_MAPPING", "1"},
		{"ENABLE_NORMAL_MAPPING", "1"},
	};

	CEArray<uint8> ShaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ForwardPass.hlsl"), "VSMain", &Defines,
	                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	VertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
	if (!VertexShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		VertexShader->SetName("ForwardPass VertexShader");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ForwardPass.hlsl"), "PSMain", &Defines,
	                                     CEShaderStage::Pixel, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	PixelShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
	if (!PixelShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PixelShader->SetName("ForwardPass PixelShader");
	}

	CEDepthStencilStateCreateInfo DepthStencilStateInfo;
	DepthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	DepthStencilStateInfo.DepthEnable = true;
	DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

	CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->
		CreateDepthStencilState(DepthStencilStateInfo);
	if (!DepthStencilState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		DepthStencilState->SetName("ForwardPass DepthStencilState");
	}

	CERasterizerStateCreateInfo RasterizerStateInfo;
	RasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
	if (!RasterizerState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RasterizerState->SetName("ForwardPass RasterizerState");
	}

	CEBlendStateCreateInfo BlendStateInfo;
	BlendStateInfo.IndependentBlendEnable = false;
	BlendStateInfo.RenderTarget[0].BlendEnable = true;

	CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
	if (!BlendState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlendState->SetName("ForwardPass BlendState");
	}

	CEGraphicsPipelineStateCreateInfo PSOProperties;
	PSOProperties.ShaderState.VertexShader = VertexShader.Get();
	PSOProperties.ShaderState.PixelShader = PixelShader.Get();
	PSOProperties.InputLayoutState = FrameResources.StdInputLayout.Get();
	PSOProperties.DepthStencilState = DepthStencilState.Get();
	PSOProperties.BlendState = BlendState.Get();
	PSOProperties.RasterizerState = RasterizerState.Get();
	PSOProperties.PipelineFormats.RenderTargetFormats[0] = FrameResources.FinalTargetFormat;
	PSOProperties.PipelineFormats.NumRenderTargets = 1;
	PSOProperties.PipelineFormats.DepthStencilFormat = FrameResources.DepthBufferFormat;
	PSOProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;

	PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PSOProperties);
	if (!PipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PipelineState->SetName("Forward PipelineState");
	}

	return true;
}

void CEDXBaseForwardRenderer::Render(
	Main::RenderLayer::CECommandList& commandList, const Main::Rendering::CEFrameResources& frameResources,
	const Main::Rendering::CELightSetup& lightSetup) {
}

void CEDXBaseForwardRenderer::Render(
	Main::RenderLayer::CECommandList& CmdList, const Main::Rendering::CEFrameResources& FrameResources,
	const Main::Rendering::CEBaseLightSetup& LightSetup) {
	// Forward Pass
	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin ForwardPass");

	TRACE_SCOPE("ForwardPass");

	const float RenderWidth = float(FrameResources.FinalTarget->GetWidth());
	const float RenderHeight = float(FrameResources.FinalTarget->GetHeight());

	CmdList.SetViewport(RenderWidth, RenderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
	CmdList.SetScissorRect(RenderWidth, RenderHeight, 0, 0);

	CERenderTargetView* FinalTargetRTV = FrameResources.FinalTarget->GetRenderTargetView();
	CmdList.SetRenderTargets(&FinalTargetRTV, 1, FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

	CmdList.SetConstantBuffer(PixelShader.Get(), FrameResources.CameraBuffer.Get(), 0);
	// TODO: Fix pointlight count in shader
	//CmdList.SetConstantBuffer(PShader.Get(), LightSetup.PointLightsBuffer.Get(), 1);
	//CmdList.SetConstantBuffer(PShader.Get(), LightSetup.PointLightsPosRadBuffer.Get(), 2);
	CmdList.SetConstantBuffer(PixelShader.Get(), LightSetup.ShadowCastingPointLightsBuffer.Get(), 1);
	CmdList.SetConstantBuffer(PixelShader.Get(), LightSetup.ShadowCastingPointLightsPosRadBuffer.Get(), 2);
	CmdList.SetConstantBuffer(PixelShader.Get(), LightSetup.DirectionalLightsBuffer.Get(), 3);

	CmdList.SetShaderResourceView(PixelShader.Get(), LightSetup.IrradianceMap->GetShaderResourceView(), 0);
	CmdList.SetShaderResourceView(PixelShader.Get(), LightSetup.SpecularIrradianceMap->GetShaderResourceView(), 1);
	CmdList.SetShaderResourceView(PixelShader.Get(), FrameResources.IntegrationLUT->GetShaderResourceView(), 2);
	CmdList.SetShaderResourceView(PixelShader.Get(), LightSetup.DirLightShadowMaps->GetShaderResourceView(), 3);
	CmdList.SetShaderResourceView(PixelShader.Get(), LightSetup.PointLightShadowMaps->GetShaderResourceView(), 4);

	CmdList.SetSamplerState(PixelShader.Get(), FrameResources.IntegrationLUTSampler.Get(), 1);
	CmdList.SetSamplerState(PixelShader.Get(), FrameResources.IrradianceSampler.Get(), 2);
	CmdList.SetSamplerState(PixelShader.Get(), FrameResources.PointShadowSampler.Get(), 3);
	CmdList.SetSamplerState(PixelShader.Get(), FrameResources.DirectionalShadowSampler.Get(), 4);

	struct TransformBuffer {
		XMFLOAT4X4 Transform;
		XMFLOAT4X4 TransformInv;
	} TransformPerObject;

	CmdList.SetGraphicsPipelineState(PipelineState.Get());
	for (const Main::Rendering::CEMeshDrawCommand& Command : FrameResources.ForwardVisibleCommands) {
		CmdList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
		CmdList.SetIndexBuffer(Command.IndexBuffer);

		if (Command.Material->IsBufferDirty()) {
			Command.Material->BuildBuffer(CmdList);
		}

		CEConstantBuffer* ConstantBuffer = Command.Material->GetMaterialBuffer();
		CmdList.SetConstantBuffer(PixelShader.Get(), ConstantBuffer, 4);

		CEShaderResourceView* const* ShaderResourceViews = Command.Material->GetShaderResourceViews();
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[0], 5);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[1], 6);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[2], 7);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[3], 8);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[4], 9);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[5], 10);
		CmdList.SetShaderResourceView(PixelShader.Get(), ShaderResourceViews[6], 11);

		CESamplerState* SamplerState = Command.Material->GetMaterialSampler();
		CmdList.SetSamplerState(PixelShader.Get(), SamplerState, 0);

		TransformPerObject.Transform = Command.CurrentActor->GetTransform().GetMatrix().Native;
		TransformPerObject.TransformInv = Command.CurrentActor->GetTransform().GetMatrixInverse().Native;

		CmdList.Set32BitShaderConstants(VertexShader.Get(), &TransformPerObject, 32);

		CmdList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
	}

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End ForwardPass");
}
