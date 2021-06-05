#include "CEDXLightProbeRenderer.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXLightProbeRenderer::Create(Main::Rendering::CELightSetup lightSetup,
                                    Main::Rendering::CEFrameResources& frameResources) {
	if (!CreateSkyLightResources(lightSetup)) {
		return false;
	}

	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/IrradianceGen.hlsl", "Main", nullptr,
	                                     Main::RenderLayer::CEShaderStage::Compute,
	                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("Failed to compile Irradiance Gen Shader");
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	IrradianceGenShader->SetName("Irradiance Gen Shader");

	IrradianceGenPSO = CastGraphicsManager()->CreateComputePipelineState(
		CEComputePipelineStateCreateInfo(IrradianceGenShader.Get()));
	if (!IrradianceGenPSO) {
		CE_LOG_ERROR("Failed to create Irradiance Gen Pipeline State");
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	IrradianceGenPSO->SetName("Irradiance Gen Pipeline State");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/SpecularIrradianceGen.hlsl", "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("Failed to compile Specular Irradiance Gen Shader");
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SpecularIrradianceGenShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!SpecularIrradianceGenShader) {
		CE_LOG_ERROR("Failed to create Specular Irradiance Gen Shader");
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SpecularIrradianceGenShader->SetName("Specular Irradiance Gen Pipeline State");

	CESamplerStateCreateInfo createInfo;
	createInfo.AddressU = CESamplerMode::Wrap;
	createInfo.AddressV = CESamplerMode::Wrap;
	createInfo.AddressW = CESamplerMode::Wrap;
	createInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;

	frameResources.IrradianceSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
	if (!frameResources.IrradianceSampler) {
		return false;
	}

	return true;
}

void CEDXLightProbeRenderer::RenderSkyLightProbe(Main::RenderLayer::CECommandList& commandList,
                                                 const Main::Rendering::CELightSetup& lightSetup,
                                                 const Main::Rendering::CEFrameResources& resources) {
}

bool CEDXLightProbeRenderer::CreateSkyLightResources(Main::Rendering::CELightSetup& lightSetup) {
	return false;
}
