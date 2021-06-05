#include "CEDXLightProbeRenderer.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#ifdef max
#undef max
#endif

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
	const uint16 irradianceSize = 32;

	lightSetup.IrradianceMap = CastGraphicsManager()->CreateTextureCube(CEFormat::R16G16B16A16_Float, irradianceSize, 1,
	                                                                    TextureFlags_RWTexture, CEResourceState::Common,
	                                                                    nullptr);
	if (!lightSetup.IrradianceMap) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	lightSetup.IrradianceMap->SetName("Irradiance Map");

	lightSetup.IrradianceMapUAV = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
		lightSetup.IrradianceMap.Get(), CEFormat::R16G16B16A16_Float, 0);
	if (!lightSetup.IrradianceMapUAV) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	const uint16 specularIrradianceSize = 128;
	const uint16 specularIrradianceMipLevels = uint16(std::max(std::log2(specularIrradianceSize), 1.0));
	lightSetup.SpecularIrradianceMap = CastGraphicsManager()->CreateTextureCube(
		CEFormat::R16G16B16A16_Float, specularIrradianceSize, specularIrradianceMipLevels, TextureFlags_RWTexture,
		CEResourceState::Common, nullptr);
	if (!lightSetup.SpecularIrradianceMap) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	lightSetup.SpecularIrradianceMap->SetName("Specular Irradiance Map");


	for (uint32 mipLevel = 0; mipLevel < specularIrradianceMipLevels; mipLevel++) {
		Core::Common::CERef<CEUnorderedAccessView> uav = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
			lightSetup.SpecularIrradianceMap.Get(), CEFormat::R16G16B16A16_Float, mipLevel);
		if (!uav) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		lightSetup.SpecularIrradianceMapUAVs.EmplaceBack(uav);
		lightSetup.WeakSpecularIrradianceMapUAVs.EmplaceBack(uav.Get());
	}

	return true;
}
