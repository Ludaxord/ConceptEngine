#include "CEDXLightProbeRenderer.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"
#include "../../../Utilities/CEDirectoryUtilities.h"

#ifdef max
#undef max
#endif

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXLightProbeRenderer::Create(Main::Rendering::CELightSetup lightSetup,
                                    Main::Rendering::CEFrameResources& frameResources) {
	if (!CreateSkyLightResources(lightSetup)) {
		return false;
	}

	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/IrradianceGen.hlsl"), "Main", nullptr,
	                                     Main::RenderLayer::CEShaderStage::Compute,
	                                     Main::RenderLayer::CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("Failed to compile Irradiance Gen Shader");
		CEDebug::DebugBreak();
		return false;
	}

	IrradianceGenShader->SetName("Irradiance Gen Shader");

	IrradianceGenPSO = CastGraphicsManager()->CreateComputePipelineState(
		CEComputePipelineStateCreateInfo(IrradianceGenShader.Get()));
	if (!IrradianceGenPSO) {
		CE_LOG_ERROR("Failed to create Irradiance Gen Pipeline State");
		CEDebug::DebugBreak();
		return false;
	}

	IrradianceGenPSO->SetName("Irradiance Gen Pipeline State");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/SpecularIrradianceGen.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("Failed to compile Specular Irradiance Gen Shader");
		CEDebug::DebugBreak();
		return false;
	}

	SpecularIrradianceGenShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!SpecularIrradianceGenShader) {
		CE_LOG_ERROR("Failed to create Specular Irradiance Gen Shader");
		CEDebug::DebugBreak();
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
	const uint32 irradianceMapSize = static_cast<uint32>(lightSetup.IrradianceMap->GetSize());

	commandList.TransitionTexture(resources.Skybox.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);
	commandList.TransitionTexture(lightSetup.IrradianceMap.Get(), CEResourceState::Common,
	                              CEResourceState::UnorderedAccess);

	commandList.SetComputePipelineState(IrradianceGenPSO.Get());

	CEShaderResourceView* skyboxSRV = resources.Skybox->GetShaderResourceView();
	commandList.SetShaderResourceView(IrradianceGenShader.Get(), skyboxSRV, 0);
	commandList.SetUnorderedAccessView(IrradianceGenShader.Get(), lightSetup.IrradianceMapUAV.Get(), 0);

	{
		const DirectX::XMUINT3 threadCount = IrradianceGenShader->GetThreadGroupXYZ().Native;
		const uint32 threadWidth = Math::CEMath::DivideByMultiple(irradianceMapSize, threadCount.x);
		const uint32 threadHeight = Math::CEMath::DivideByMultiple(irradianceMapSize, threadCount.y);
		commandList.Dispatch(threadWidth, threadHeight, 6);
	}

	commandList.UnorderedAccessTextureBarrier(lightSetup.IrradianceMap.Get());

	commandList.TransitionTexture(lightSetup.IrradianceMap.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::PixelShaderResource);
	commandList.TransitionTexture(lightSetup.SpecularIrradianceMap.Get(), CEResourceState::Common,
	                              CEResourceState::UnorderedAccess);

	commandList.SetShaderResourceView(IrradianceGenShader.Get(), skyboxSRV, 0);

	commandList.SetComputePipelineState(SpecularIrradianceGenPSO.Get());

	uint32 width = lightSetup.SpecularIrradianceMap->GetSize();
	float roughness = 0.0f;

	const uint32 numMipLevels = lightSetup.SpecularIrradianceMap->GetNumMips();
	const float roughnessDelta = 1.0f / (numMipLevels - 1);
	for (uint32 mip = 0; mip < numMipLevels; mip++) {
		commandList.Set32BitShaderConstants(SpecularIrradianceGenShader.Get(), &roughness, 1);
		commandList.SetUnorderedAccessView(SpecularIrradianceGenShader.Get(),
		                                   lightSetup.SpecularIrradianceMapUAVs[mip].Get(), 0);

		{
			const DirectX::XMUINT3 threadCount = SpecularIrradianceGenShader->GetThreadGroupXYZ().Native;
			const uint32 threadWidth = Math::CEMath::DivideByMultiple(width, threadCount.x);
			const uint32 threadHeight = Math::CEMath::DivideByMultiple(width, threadCount.y);
			commandList.Dispatch(threadWidth, threadHeight, 6);
		}

		commandList.UnorderedAccessTextureBarrier(lightSetup.SpecularIrradianceMap.Get());

		width = std::max<uint32>(width / 2, 1U);
		roughness += roughnessDelta;
	}

	commandList.TransitionTexture(resources.Skybox.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	commandList.TransitionTexture(lightSetup.SpecularIrradianceMap.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::PixelShaderResource);
}

bool CEDXLightProbeRenderer::CreateSkyLightResources(Main::Rendering::CELightSetup& lightSetup) {
	const uint16 irradianceSize = 32;

	lightSetup.IrradianceMap = CastGraphicsManager()->CreateTextureCube(CEFormat::R16G16B16A16_Float, irradianceSize, 1,
	                                                                    TextureFlags_RWTexture, CEResourceState::Common,
	                                                                    nullptr);
	if (!lightSetup.IrradianceMap) {
		CEDebug::DebugBreak();
		return false;
	}

	lightSetup.IrradianceMap->SetName("Irradiance Map");

	lightSetup.IrradianceMapUAV = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
		lightSetup.IrradianceMap.Get(), CEFormat::R16G16B16A16_Float, 0);
	if (!lightSetup.IrradianceMapUAV) {
		CEDebug::DebugBreak();
		return false;
	}

	const uint16 specularIrradianceSize = 128;
	const uint16 specularIrradianceMipLevels = uint16(std::max(std::log2(specularIrradianceSize), 1.0));
	lightSetup.SpecularIrradianceMap = CastGraphicsManager()->CreateTextureCube(
		CEFormat::R16G16B16A16_Float, specularIrradianceSize, specularIrradianceMipLevels, TextureFlags_RWTexture,
		CEResourceState::Common, nullptr);
	if (!lightSetup.SpecularIrradianceMap) {
		CEDebug::DebugBreak();
		return false;
	}

	lightSetup.SpecularIrradianceMap->SetName("Specular Irradiance Map");


	for (uint32 mipLevel = 0; mipLevel < specularIrradianceMipLevels; mipLevel++) {
		CERef<CEUnorderedAccessView> uav = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
			lightSetup.SpecularIrradianceMap.Get(), CEFormat::R16G16B16A16_Float, mipLevel);
		if (!uav) {
			CEDebug::DebugBreak();
			return false;
		}

		lightSetup.SpecularIrradianceMapUAVs.EmplaceBack(uav);
		lightSetup.WeakSpecularIrradianceMapUAVs.EmplaceBack(uav.Get());
	}

	return true;
}
