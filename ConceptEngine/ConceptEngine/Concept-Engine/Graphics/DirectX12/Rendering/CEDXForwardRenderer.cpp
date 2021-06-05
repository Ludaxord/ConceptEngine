#include "CEDXForwardRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXForwardRenderer::Create(const Main::Rendering::CEFrameResources& resources) {

	Core::Containers::CEArray<Main::RenderLayer::CEShaderDefine> defines = {
		{"ENABLE_PARALLAX_MAPPING", "1"},
		{"ENABLE_NORMAL_MAPPING", "1"}
	};

	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ForwardPass.hlsl",
	                                     "VSMain",
	                                     &defines,
	                                     Main::RenderLayer::CEShaderStage::Vertex,
	                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	VertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!VertexShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	VertexShader->SetName("Forward Pass Vertex Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ForwardPass.hlsl", "PSMain", &defines, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!PixelShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PixelShader->SetName("Forward Pass Pixel Shader");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	depthStencilStateInfo.DepthEnable = true;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

	Core::Common::CERef<CEDepthStencilState> depthStencilState = CastGraphicsManager()->CreateDepthStencilState(
		depthStencilStateInfo);
	if (!depthStencilState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	depthStencilState->SetName("Forward Pass Depth Stencil State");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	Core::Common::CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(
		rasterizerStateInfo);
	if (!rasterizerState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	rasterizerState->SetName("Forward Pass Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;
	blendStateInfo.independentBlendEnable = false;
	blendStateInfo.RenderTarget[0].BlendEnable = true;

	Core::Common::CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);
	if (!blendState) {
		Core::Debug::CEDebug::DebugBreak();
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
		Core::Debug::CEDebug::DebugBreak();
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

	//TODO: Finish!

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END FORWARD PASS ==");
}
