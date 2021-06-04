#include "CEDXShadowMapRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

struct PerShadowMap {
	DirectX::XMFLOAT4X4 Matrix;
	DirectX::XMFLOAT3 Position;
	float FarPlane;
};

bool CEDXShadowMapRenderer::Create(Main::Rendering::CELightSetup& lightSetup,
                                   Main::Rendering::CEFrameResources& resources) {
	if (!CreateShadowMaps(lightSetup)) {
		return false;
	}

	PerShadowMapBuffer = CastGraphicsManager()->CreateConstantBuffer<PerShadowMap>(
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);
	if (!PerShadowMapBuffer) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PerShadowMapBuffer->SetName("Per Shadow Map Buffer");

	Core::Containers::CEArray<uint8> shaderCode;
	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "VSMain", nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!PointLightVertexShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader->SetName("Linear Shadow Map Vertex Shader");

		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "PSMain", nullptr,
		                                     RenderLayer::CEShaderStage::Pixel, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
		if (!PointLightPixelShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader->SetName("Linear Shadow Map Pixel Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		Core::Common::CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;

		Core::Common::CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Shadow Blend State");

		RenderLayer::CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
		pipelineStateInfo.BlendState = blendState.Get();
		pipelineStateInfo.DepthStencilState = depthStencilState.Get();
		pipelineStateInfo.IBStripCutValue = RenderLayer::CEIndexBufferStripCutValue::Disabled;
		pipelineStateInfo.InputLayoutState = resources.StdInputLayout.Get();
		pipelineStateInfo.PrimitiveTopologyType = RenderLayer::CEPrimitiveTopologyType::Triangle;
		pipelineStateInfo.RasterizerState = rasterizerState.Get();
		pipelineStateInfo.SampleCount = 1;
		pipelineStateInfo.SampleQuality = 0;
		pipelineStateInfo.SampleMask = 0xffffffff;
		pipelineStateInfo.ShaderState.VertexShader = PointLightVertexShader.Get();
		pipelineStateInfo.ShaderState.PixelShader = PointLightPixelShader.Get();
		pipelineStateInfo.PipelineFormats.NumRenderTargets = 0;
		pipelineStateInfo.PipelineFormats.DepthStencilFormat = lightSetup.ShadowMapFormat;

		PointLightPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(pipelineStateInfo);
		if (!PointLightPipelineState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPipelineState->SetName("Linear Shadow Map Pipeline State");
	}

	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "Main", nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		DirLightShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!DirLightShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		DirLightShader->SetName("Shadow Map Vertex Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		Core::Common::CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		Core::Common::CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Shadow Blend State");
	}

	return true;
}

void CEDXShadowMapRenderer::RenderPointLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                    const Main::Rendering::CELightSetup& lightSetup,
                                                    const Render::Scene::CEScene& scene) {
}

void CEDXShadowMapRenderer::RenderDirectionalLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                          const Main::Rendering::CELightSetup& lightSetup,
                                                          const Render::Scene::CEScene& scene) {
}

bool CEDXShadowMapRenderer::CreateShadowMaps(Main::Rendering::CELightSetup& frameResources) {
	return false;
}
