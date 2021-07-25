#include "CEDXBaseScreenSpaceOcclusionRenderer.h"

#include <random>

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"

#include "../../../../Math/CEFloat.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAORadius(0.3f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAOBias(0.03f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<int32> GSSAOKernelSize(32);

bool CEDXBaseScreenSpaceOcclusionRenderer::Create(Main::Rendering::CEFrameResources& FrameResources) {
	INIT_CONSOLE_VARIABLE("CE.SSAOKernelSize", &GSSAOKernelSize);
	INIT_CONSOLE_VARIABLE("CE.SSAOBias", &GSSAOBias);
	INIT_CONSOLE_VARIABLE("CE.SSAORadius", &GSSAORadius);


	if (!CreateRenderTarget(FrameResources)) {
		return false;
	}

	CEArray<uint8> ShaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/SSAO.hlsl"), "Main", nullptr, CEShaderStage::Compute,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAOShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
	if (!SSAOShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		SSAOShader->SetName("SSAO Shader");
	}

	CEComputePipelineStateCreateInfo PipelineStateInfo;
	PipelineStateInfo.Shader = SSAOShader.Get();

	PipelineState = CastGraphicsManager()->CreateComputePipelineState(PipelineStateInfo);
	if (!PipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PipelineState->SetName("SSAO PipelineState");
	}

	// Generate SSAO Kernel
	std::uniform_real_distribution<float> RandomFloats(0.0f, 1.0f);
	std::default_random_engine Generator;

	XMVECTOR Normal = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	CEArray<XMFLOAT3> SSAOKernel;
	for (uint32 i = 0; i < 256 && SSAOKernel.Size() < 64; ++i) {
		XMVECTOR XmSample = XMVectorSet(RandomFloats(Generator) * 2.0f - 1.0f, RandomFloats(Generator) * 2.0f - 1.0f,
		                                RandomFloats(Generator), 0.0f);

		float Scale = RandomFloats(Generator);
		XmSample = XMVector3Normalize(XmSample);
		XmSample = XMVectorScale(XmSample, Scale);

		float Dot = XMVectorGetX(XMVector3Dot(XmSample, Normal));
		if (Math::CEMath::Abs(Dot) > 0.85f) {
			continue;
		}

		Scale = float(i) / 64.0f;
		Scale = Math::CEMath::Lerp(0.1f, 1.0f, Scale * Scale);
		XmSample = XMVectorScale(XmSample, Scale);

		XMFLOAT3 Sample;
		XMStoreFloat3(&Sample, XmSample);
		SSAOKernel.EmplaceBack(Sample);
	}

	// Generate noise
	CEArray<CEFloat16> SSAONoise;
	for (uint32 i = 0; i < 16; i++) {
		const float x = RandomFloats(Generator) * 2.0f - 1.0f;
		const float y = RandomFloats(Generator) * 2.0f - 1.0f;
		SSAONoise.EmplaceBack(x);
		SSAONoise.EmplaceBack(y);
		SSAONoise.EmplaceBack(0.0f);
		SSAONoise.EmplaceBack(0.0f);
	}

	SSAONoiseTexture = CastGraphicsManager()->CreateTexture2D(CEFormat::R16G16B16A16_Float,
	                                                          4,
	                                                          4,
	                                                          1,
	                                                          1,
	                                                          TextureFlag_SRV,
	                                                          CEResourceState::NonPixelShaderResource,
	                                                          nullptr);

	if (!SSAONoiseTexture) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		SSAONoiseTexture->SetName("SSAO Noise Texture");
	}

	CECommandList CmdList;
	CmdList.Begin();

	CmdList.TransitionTexture(FrameResources.SSAOBuffer.Get(), CEResourceState::Common,
	                          CEResourceState::NonPixelShaderResource);
	CmdList.TransitionTexture(SSAONoiseTexture.Get(), CEResourceState::NonPixelShaderResource,
	                          CEResourceState::CopyDest);

	CmdList.UpdateTexture2D(SSAONoiseTexture.Get(), 4, 4, 0, SSAONoise.Data());

	CmdList.TransitionTexture(SSAONoiseTexture.Get(), CEResourceState::CopyDest,
	                          CEResourceState::NonPixelShaderResource);

	CmdList.End();
	CommandListExecutor.ExecuteCommandList(CmdList);

	const uint32 Stride = sizeof(XMFLOAT3);
	CEResourceData SSAOSampleData(SSAOKernel.Data(), SSAOKernel.SizeInBytes());
	SSAOSamples = CastGraphicsManager()->CreateStructuredBuffer(Stride,
	                                                            SSAOKernel.Size(),
	                                                            BufferFlag_SRV | BufferFlag_Default,
	                                                            CEResourceState::Common,
	                                                            &SSAOSampleData);
	if (!SSAOSamples) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		SSAOSamples->SetName("SSAO Samples");
	}

	SSAOSamplesSRV = CastGraphicsManager()->CreateShaderResourceViewForStructuredBuffer(
		SSAOSamples.Get(), 0, SSAOKernel.Size());
	if (!SSAOSamplesSRV) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		SSAOSamplesSRV->SetName("SSAO Samples SRV");
	}

	CEArray<CEShaderDefine> Defines;
	Defines.EmplaceBack("HORIZONTAL_PASS", "1");

	// Load shader
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Blur.hlsl"), "Main", &Defines, CEShaderStage::Compute,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurHorizontalShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
	if (!BlurHorizontalShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlurHorizontalShader->SetName("SSAO Horizontal Blur Shader");
	}

	CEComputePipelineStateCreateInfo PSOProperties;
	PSOProperties.Shader = BlurHorizontalShader.Get();

	BlurHorizontalPSO = CastGraphicsManager()->CreateComputePipelineState(PSOProperties);
	if (!BlurHorizontalPSO) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlurHorizontalPSO->SetName("SSAO Horizontal Blur PSO");
	}

	Defines.Clear();
	Defines.EmplaceBack("VERTICAL_PASS", "1");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Blur.hlsl"), "Main", &Defines, CEShaderStage::Compute,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurVerticalShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
	if (!BlurVerticalShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlurVerticalShader->SetName("SSAO Vertcial Blur Shader");
	}

	PSOProperties.Shader = BlurVerticalShader.Get();

	BlurVerticalPSO = CastGraphicsManager()->CreateComputePipelineState(PSOProperties);
	if (!BlurVerticalPSO) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlurVerticalPSO->SetName("SSAO Vertical Blur PSO");
	}

	return true;
}

void CEDXBaseScreenSpaceOcclusionRenderer::Render(RenderLayer::CECommandList& CmdList,
                                                  Main::Rendering::CEFrameResources& FrameResources) {
	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin SSAO");

	TRACE_SCOPE("SSAO");

	CmdList.SetComputePipelineState(PipelineState.Get());

	struct SSAOSettings {
		XMFLOAT2 ScreenSize;
		XMFLOAT2 NoiseSize;
		float Radius;
		float Bias;
		int32 KernelSize;
	} SSAOSettings;

	const uint32 Width = FrameResources.SSAOBuffer->GetWidth();
	const uint32 Height = FrameResources.SSAOBuffer->GetHeight();
	SSAOSettings.ScreenSize = XMFLOAT2(float(Width), float(Height));
	SSAOSettings.NoiseSize = XMFLOAT2(4.0f, 4.0f);
	SSAOSettings.Radius = GSSAORadius.GetFloat();
	SSAOSettings.KernelSize = GSSAOKernelSize.GetInt();
	SSAOSettings.Bias = GSSAOBias.GetFloat();

	CmdList.SetConstantBuffer(SSAOShader.Get(), FrameResources.CameraBuffer.Get(), 0);

	FrameResources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(SSAONoiseTexture->GetShaderResourceView()),
		SSAONoiseTexture,
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CmdList.SetShaderResourceView(SSAOShader.Get(),
	                              FrameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetShaderResourceView(), 0);
	CmdList.SetShaderResourceView(SSAOShader.Get(), FrameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView(),
	                              1);
	CmdList.SetShaderResourceView(SSAOShader.Get(), SSAONoiseTexture->GetShaderResourceView(), 2);
	CmdList.SetShaderResourceView(SSAOShader.Get(), SSAOSamplesSRV.Get(), 3);

	CmdList.SetSamplerState(SSAOShader.Get(), FrameResources.GBufferSampler.Get(), 0);

	CEUnorderedAccessView* SSAOBufferUAV = FrameResources.SSAOBuffer->GetUnorderedAccessView();
	CmdList.SetUnorderedAccessView(SSAOShader.Get(), SSAOBufferUAV, 0);
	CmdList.Set32BitShaderConstants(SSAOShader.Get(), &SSAOSettings, 7);

	constexpr uint32 ThreadCount = 16;
	const uint32 DispatchWidth = Math::CEMath::DivideByMultiple<uint32>(Width, ThreadCount);
	const uint32 DispatchHeight = Math::CEMath::DivideByMultiple<uint32>(Height, ThreadCount);
	CmdList.Dispatch(DispatchWidth, DispatchHeight, 1);

	CmdList.UnorderedAccessTextureBarrier(FrameResources.SSAOBuffer.Get());

	CmdList.SetComputePipelineState(BlurHorizontalPSO.Get());

	CmdList.Set32BitShaderConstants(BlurHorizontalShader.Get(), &SSAOSettings.ScreenSize, 2);

	CmdList.Dispatch(DispatchWidth, DispatchHeight, 1);

	CmdList.UnorderedAccessTextureBarrier(FrameResources.SSAOBuffer.Get());

	CmdList.SetComputePipelineState(BlurVerticalPSO.Get());

	CmdList.Set32BitShaderConstants(BlurVerticalShader.Get(), &SSAOSettings.ScreenSize, 2);

	CmdList.Dispatch(DispatchWidth, DispatchHeight, 1);

	CmdList.UnorderedAccessTextureBarrier(FrameResources.SSAOBuffer.Get());

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End SSAO");
}

bool CEDXBaseScreenSpaceOcclusionRenderer::ResizeResources(Main::Rendering::CEFrameResources& FrameResources) {
	return CreateRenderTarget(FrameResources);
}

bool CEDXBaseScreenSpaceOcclusionRenderer::CreateRenderTarget(
	Main::Rendering::CEFrameResources& FrameResources) {
	const uint32 Width = FrameResources.MainWindowViewport->GetWidth();
	const uint32 Height = FrameResources.MainWindowViewport->GetHeight();
	const uint32 Flags = TextureFlags_RWTexture;

	FrameResources.SSAOBuffer = CastGraphicsManager()->CreateTexture2D(FrameResources.SSAOBufferFormat, Width, Height,
	                                                                   1, 1, Flags,
	                                                                   CEResourceState::Common, nullptr);
	if (!FrameResources.SSAOBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FrameResources.SSAOBuffer->SetName("SSAO Buffer");
	}

	return true;
}
