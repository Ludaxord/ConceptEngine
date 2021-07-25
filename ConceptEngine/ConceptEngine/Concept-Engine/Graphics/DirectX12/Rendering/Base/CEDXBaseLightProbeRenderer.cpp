#include "CEDXBaseLightProbeRenderer.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"

#ifdef max
#undef max
#endif

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

bool CEDXBaseLightProbeRenderer::Create(
	Main::Rendering::CELightSetup& lightSetup, Main::Rendering::CEFrameResources& frameResources) {
	return false;
}

void CEDXBaseLightProbeRenderer::RenderSkyLightProbe(
	CECommandList& commandList, const Main::Rendering::CELightSetup& lightSetup,
	const Main::Rendering::CEFrameResources& resources) {
}

bool CEDXBaseLightProbeRenderer::CreateSkyLightResources(
	Main::Rendering::CELightSetup& lightSetup) {
	return false;
}

bool CEDXBaseLightProbeRenderer::CreateSkyLightResources(Main::Rendering::CEBaseLightSetup& LightSetup) {
	// Generate global irradiance (From Skybox)
	const uint16 IrradianceSize = 32;
	LightSetup.IrradianceMap = CastGraphicsManager()->CreateTextureCube(CEFormat::R16G16B16A16_Float, IrradianceSize, 1,
	                                                                    TextureFlags_RWTexture, CEResourceState::Common,
	                                                                    nullptr);
	if (!LightSetup.IrradianceMap) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		LightSetup.IrradianceMap->SetName("Irradiance Map");
	}

	LightSetup.IrradianceMapUAV = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
		LightSetup.IrradianceMap.Get(), CEFormat::R16G16B16A16_Float, 0);
	if (!LightSetup.IrradianceMapUAV) {
		CEDebug::DebugBreak();
		return false;
	}

	const uint16 SpecularIrradianceSize = 128;
	const uint16 SpecularIrradianceMiplevels = uint16(std::max(std::log2(SpecularIrradianceSize), 1.0));
	LightSetup.SpecularIrradianceMap = CastGraphicsManager()->CreateTextureCube(
		CEFormat::R16G16B16A16_Float,
		SpecularIrradianceSize,
		SpecularIrradianceMiplevels,
		TextureFlags_RWTexture,
		CEResourceState::Common,
		nullptr);
	if (!LightSetup.SpecularIrradianceMap) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		LightSetup.SpecularIrradianceMap->SetName("Specular Irradiance Map");
	}

	for (uint32 MipLevel = 0; MipLevel < SpecularIrradianceMiplevels; MipLevel++) {
		CERef<CEUnorderedAccessView> Uav = CastGraphicsManager()->CreateUnorderedAccessViewForTextureCube(
			LightSetup.SpecularIrradianceMap.Get(),
			CEFormat::R16G16B16A16_Float, MipLevel);
		if (Uav) {
			LightSetup.SpecularIrradianceMapUAVs.EmplaceBack(Uav);
			LightSetup.WeakSpecularIrradianceMapUAVs.EmplaceBack(Uav.Get());
		}
		else {
			CEDebug::DebugBreak();
			return false;
		}
	}

	return true;
}

bool CEDXBaseLightProbeRenderer::Create(
	Main::Rendering::CEBaseLightSetup& LightSetup, Main::Rendering::CEFrameResources& FrameResources) {
	if (!CreateSkyLightResources(LightSetup)) {
		return false;
	}

	CEArray<uint8> Code;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/IrradianceGen.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, Code)) {
		CE_LOG_ERROR("Failed to compile IrradianceGen Shader");
		CEDebug::DebugBreak();
	}

	IrradianceGenShader = CastGraphicsManager()->CreateComputeShader(Code);
	if (!IrradianceGenShader) {
		CE_LOG_ERROR("Failed to create IrradianceGen Shader");
		CEDebug::DebugBreak();
	}
	else {
		IrradianceGenShader->SetName("IrradianceGen Shader");
	}

	IrradianceGenPSO = CastGraphicsManager()->CreateComputePipelineState(
		CEComputePipelineStateCreateInfo(IrradianceGenShader.Get()));
	if (!IrradianceGenPSO) {
		CE_LOG_ERROR("Failed to create IrradianceGen PipelineState");
		CEDebug::DebugBreak();
	}
	else {
		IrradianceGenPSO->SetName("IrradianceGen PSO");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/SpecularIrradianceGen.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, Code)) {
		CE_LOG_ERROR("Failed to compile SpecularIrradianceGen Shader");
		CEDebug::DebugBreak();
	}

	SpecularIrradianceGenShader = CastGraphicsManager()->CreateComputeShader(Code);
	if (!SpecularIrradianceGenShader) {
		CE_LOG_ERROR("Failed to create Specular IrradianceGen Shader");
		CEDebug::DebugBreak();
	}
	else {
		SpecularIrradianceGenShader->SetName("Specular IrradianceGen Shader");
	}

	SpecularIrradianceGenPSO = CastGraphicsManager()->CreateComputePipelineState(
		CEComputePipelineStateCreateInfo(SpecularIrradianceGenShader.Get()));
	if (!SpecularIrradianceGenPSO) {
		CE_LOG_ERROR("Failed to create Specular IrradianceGen PipelineState");
		CEDebug::DebugBreak();
	}
	else {
		SpecularIrradianceGenPSO->SetName("Specular IrradianceGen PSO");
	}

	CESamplerStateCreateInfo CreateInfo;
	CreateInfo.AddressU = CESamplerMode::Wrap;
	CreateInfo.AddressV = CESamplerMode::Wrap;
	CreateInfo.AddressW = CESamplerMode::Wrap;
	CreateInfo.Filter = CESamplerFilter::MinMagMipLinear;

	FrameResources.IrradianceSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
	if (!FrameResources.IrradianceSampler) {
		return false;
	}

	return true;
}

void CEDXBaseLightProbeRenderer::RenderSkyLightProbe(
	CECommandList& CmdList, const Main::Rendering::CEBaseLightSetup& LightSetup,
	const Main::Rendering::CEFrameResources& FrameResources) {
	const uint32 IrradianceMapSize = static_cast<uint32>(LightSetup.IrradianceMap->GetSize());

	CmdList.TransitionTexture(FrameResources.Skybox.Get(), CEResourceState::PixelShaderResource,
	                          CEResourceState::NonPixelShaderResource);
	CmdList.TransitionTexture(LightSetup.IrradianceMap.Get(), CEResourceState::Common,
	                          CEResourceState::UnorderedAccess);

	CmdList.SetComputePipelineState(IrradianceGenPSO.Get());

	CEShaderResourceView* SkyboxSRV = FrameResources.Skybox->GetShaderResourceView();
	CmdList.SetShaderResourceView(IrradianceGenShader.Get(), SkyboxSRV, 0);
	CmdList.SetUnorderedAccessView(IrradianceGenShader.Get(), LightSetup.IrradianceMapUAV.Get(), 0);

	{
		const XMUINT3 ThreadCount = IrradianceGenShader->GetThreadGroupXYZ().Native;
		const uint32 ThreadWidth = Math::CEMath::DivideByMultiple(IrradianceMapSize, ThreadCount.x);
		const uint32 ThreadHeight = Math::CEMath::DivideByMultiple(IrradianceMapSize, ThreadCount.y);
		CmdList.Dispatch(ThreadWidth, ThreadHeight, 6);
	}

	CmdList.UnorderedAccessTextureBarrier(LightSetup.IrradianceMap.Get());

	CmdList.TransitionTexture(LightSetup.IrradianceMap.Get(), CEResourceState::UnorderedAccess,
	                          CEResourceState::PixelShaderResource);
	CmdList.TransitionTexture(LightSetup.SpecularIrradianceMap.Get(), CEResourceState::Common,
	                          CEResourceState::UnorderedAccess);

	CmdList.SetShaderResourceView(IrradianceGenShader.Get(), SkyboxSRV, 0);

	CmdList.SetComputePipelineState(SpecularIrradianceGenPSO.Get());

	uint32 Width = LightSetup.SpecularIrradianceMap->GetSize();
	float Roughness = 0.0f;

	const uint32 NumMiplevels = LightSetup.SpecularIrradianceMap->GetNumMips();
	const float RoughnessDelta = 1.0f / (NumMiplevels - 1);
	for (uint32 Mip = 0; Mip < NumMiplevels; Mip++) {
		CmdList.Set32BitShaderConstants(SpecularIrradianceGenShader.Get(), &Roughness, 1);
		CmdList.SetUnorderedAccessView(SpecularIrradianceGenShader.Get(),
		                               LightSetup.SpecularIrradianceMapUAVs[Mip].Get(), 0);

		{
			const XMUINT3 ThreadCount = SpecularIrradianceGenShader->GetThreadGroupXYZ().Native;
			const uint32 ThreadWidth = Math::CEMath::DivideByMultiple(Width, ThreadCount.x);
			const uint32 ThreadHeight = Math::CEMath::DivideByMultiple(Width, ThreadCount.y);
			CmdList.Dispatch(ThreadWidth, ThreadHeight, 6);
		}

		CmdList.UnorderedAccessTextureBarrier(LightSetup.SpecularIrradianceMap.Get());

		Width = std::max<uint32>(Width / 2, 1U);
		Roughness += RoughnessDelta;
	}

	CmdList.TransitionTexture(FrameResources.Skybox.Get(), CEResourceState::NonPixelShaderResource,
	                          CEResourceState::PixelShaderResource);
	CmdList.TransitionTexture(LightSetup.SpecularIrradianceMap.Get(), CEResourceState::UnorderedAccess,
	                          CEResourceState::PixelShaderResource);
}
