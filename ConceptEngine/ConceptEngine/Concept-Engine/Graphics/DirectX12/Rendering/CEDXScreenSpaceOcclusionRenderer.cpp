#include "CEDXScreenSpaceOcclusionRenderer.h"

#include <random>

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Math/CEFloat.h"
#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAORadius(0.3f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAOBias(0.03f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<int32> GSSAOKernelSize(32);

bool CEDXScreenSpaceOcclusionRenderer::Create(Main::Rendering::CEFrameResources& resources) {
	INIT_CONSOLE_VARIABLE("CE.SSAOKernelSize", &GSSAOKernelSize);
	INIT_CONSOLE_VARIABLE("CE.SSAOBias", &GSSAOBias);
	INIT_CONSOLE_VARIABLE("CE.SSAORadius", &GSSAORadius);

	if (!CreateRenderTarget(resources)) {
		return false;
	}

	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/SSAO.hlsl"), "Main", nullptr,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAOShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!SSAOShader) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAOShader->SetName("SSAO Shader");

	RenderLayer::CEComputePipelineStateCreateInfo pipelineStateCreateInfo;
	pipelineStateCreateInfo.Shader = SSAOShader.Get();

	PipelineState = CastGraphicsManager()->CreateComputePipelineState(pipelineStateCreateInfo);
	if (!PipelineState) {
		CEDebug::DebugBreak();
		return false;
	}

	PipelineState->SetName("SSAO Pipeline State");

	//TODO: Change all constants digits into editor option modifiers.

	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	CEArray<DirectX::XMFLOAT3> SSAOKernel;
	for (uint32 i = 0; i < 256 && SSAOKernel.Size() < 64; ++i) {
		DirectX::XMVECTOR xmSample = DirectX::XMVectorSet(randomFloats(generator) * 2.0f - 1.0f,
		                                                  randomFloats(generator) * 2.0f - 1.0f,
		                                                  randomFloats(generator), 0.0f);

		float scale = randomFloats(generator);
		xmSample = DirectX::XMVector3Normalize(xmSample);
		xmSample = DirectX::XMVectorScale(xmSample, scale);

		float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(xmSample, normal));
		if (Math::CEMath::Abs(dot) > 0.85f) {
			continue;
		}

		scale = float(i) / 64.0f;
		scale = Math::CEMath::Lerp(0.1f, 1.0f, scale * scale);
		xmSample = DirectX::XMVectorScale(xmSample, scale);

		DirectX::XMFLOAT3 sample;
		DirectX::XMStoreFloat3(&sample, xmSample);
		SSAOKernel.EmplaceBack(sample);
	}

	//Generate noise
	CEArray<CEFloat16> SSAONoise;
	for (uint32 i = 0; i < 16; i++) {
		const float x = randomFloats(generator) * 2.0f - 1.0f;
		const float y = randomFloats(generator) * 2.0f - 1.0f;

		SSAONoise.EmplaceBack(x);
		SSAONoise.EmplaceBack(y);
		SSAONoise.EmplaceBack(0.0f);
		SSAONoise.EmplaceBack(0.0f);
	}

	SSAONoiseTexture = CastGraphicsManager()->CreateTexture2D(RenderLayer::CEFormat::R16G16B16A16_Float, 4, 4, 1, 1,
	                                                          RenderLayer::TextureFlag_SRV,
	                                                          RenderLayer::CEResourceState::NonPixelShaderResource,
	                                                          nullptr);
	if (!SSAONoiseTexture) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAONoiseTexture->SetName("SSAO Noise Texture");

	RenderLayer::CECommandList commandList;
	commandList.Execute([&commandList, &resources, this, &SSAONoise] {
		commandList.TransitionTexture(resources.SSAOBuffer.Get(), RenderLayer::CEResourceState::Common,
		                              RenderLayer::CEResourceState::NonPixelShaderResource);
		commandList.TransitionTexture(SSAONoiseTexture.Get(), RenderLayer::CEResourceState::NonPixelShaderResource,
		                              RenderLayer::CEResourceState::CopyDest);
		commandList.UpdateTexture2D(SSAONoiseTexture.Get(), 4, 4, 0, SSAONoise.Data());
		commandList.TransitionTexture(SSAONoiseTexture.Get(), RenderLayer::CEResourceState::CopyDest,
		                              RenderLayer::CEResourceState::NonPixelShaderResource);
	});

	CommandListExecutor.ExecuteCommandList(commandList);

	const uint32 stride = sizeof(DirectX::XMFLOAT3);
	RenderLayer::CEResourceData SSAOSampleData(SSAOKernel.Data(), SSAOKernel.SizeInBytes());
	SSAOSamples = CastGraphicsManager()->CreateStructuredBuffer(stride, SSAOKernel.Size(),
	                                                            RenderLayer::BufferFlag_SRV |
	                                                            RenderLayer::BufferFlag_Default,
	                                                            RenderLayer::CEResourceState::Common, &SSAOSampleData);
	if (!SSAOSamples) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAOSamples->SetName("SSAO Samples");

	SSAOSamplesSRV = CastGraphicsManager()->CreateShaderResourceViewForStructuredBuffer(
		SSAOSamples.Get(), 0, SSAOKernel.Size());
	if (!SSAOSamplesSRV) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAOSamplesSRV->SetName("SSAO Samples SRV");

	CEArray<RenderLayer::CEShaderDefine> defines;
	defines.EmplaceBack("HORIZONTAL_PASS", "1");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Blur.hlsl"), "Main", &defines,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurHorizontalShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!BlurHorizontalShader) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurHorizontalShader->SetName("SSAO Horizontal Blur Shader");

	RenderLayer::CEComputePipelineStateCreateInfo psoProperties;
	psoProperties.Shader = BlurHorizontalShader.Get();

	BlurHorizontalPSO = CastGraphicsManager()->CreateComputePipelineState(psoProperties);
	if (!BlurHorizontalPSO) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurHorizontalPSO->SetName("SSAO Horizontal Blur Pipeline State");

	defines.Clear();
	defines.EmplaceBack("VERTICAL_PASS", "1");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Blur.hlsl"), "Main", &defines,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurVerticalShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!BlurVerticalShader) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurVerticalShader->SetName("SSAO Vertical Blur Shader");

	psoProperties.Shader = BlurVerticalShader.Get();

	BlurVerticalPSO = CastGraphicsManager()->CreateComputePipelineState(psoProperties);
	if (!BlurVerticalPSO) {
		CEDebug::DebugBreak();
		return false;
	}

	BlurVerticalPSO->SetName("SSAO Vertical Blur Pipeline State");

	return true;
}

void CEDXScreenSpaceOcclusionRenderer::Render(Main::RenderLayer::CECommandList& commandList,
                                              Main::Rendering::CEFrameResources& frameResources) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN SSAO ==");

	TRACE_SCOPE("== SSAO ==");

	commandList.SetComputePipelineState(PipelineState.Get());

	struct SSAOSettings {
		DirectX::XMFLOAT2 ScreenSize;
		DirectX::XMFLOAT2 NoiseSize;
		float Radius;
		float Bias;
		int32 KernelSize;
	} SSAOSettings;

	const uint32 width = frameResources.SSAOBuffer->GetWidth();
	const uint32 height = frameResources.SSAOBuffer->GetHeight();
	SSAOSettings.ScreenSize = DirectX::XMFLOAT2(float(width), float(height));
	SSAOSettings.NoiseSize = DirectX::XMFLOAT2(4.0f, 4.0f);
	SSAOSettings.Radius = GSSAORadius.GetFloat();
	SSAOSettings.KernelSize = GSSAOKernelSize.GetInt();
	SSAOSettings.Bias = GSSAOBias.GetFloat();

	commandList.SetConstantBuffer(SSAOShader.Get(), frameResources.CameraBuffer.Get(), 0);

	frameResources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(SSAONoiseTexture->GetShaderResourceView()),
		SSAONoiseTexture,
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource
	);

	commandList.SetShaderResourceView(SSAOShader.Get(),
	                                  frameResources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetShaderResourceView(), 0);
	commandList.SetShaderResourceView(SSAOShader.Get(),
	                                  frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView(), 1);
	commandList.SetShaderResourceView(SSAOShader.Get(), SSAONoiseTexture->GetShaderResourceView(), 2);
	commandList.SetShaderResourceView(SSAOShader.Get(), SSAOSamplesSRV.Get(), 3);

	commandList.SetSamplerState(SSAOShader.Get(), frameResources.GBufferSampler.Get(), 0);

	CEUnorderedAccessView* ssaoBufferUAV = frameResources.SSAOBuffer->GetUnorderedAccessView();
	commandList.SetUnorderedAccessView(SSAOShader.Get(), ssaoBufferUAV, 0);
	commandList.Set32BitShaderConstants(SSAOShader.Get(), &SSAOSettings, 7);

	constexpr uint32 threadCount = 16;
	const uint32 dispatchWidth = Math::CEMath::DivideByMultiple<uint32>(width, threadCount);
	const uint32 dispatchHeight = Math::CEMath::DivideByMultiple<uint32>(height, threadCount);

	//Horizontal Blur
	commandList.Dispatch(dispatchWidth, dispatchHeight, 1);

	commandList.UnorderedAccessTextureBarrier(frameResources.SSAOBuffer.Get());

	commandList.SetComputePipelineState(BlurHorizontalPSO.Get());

	commandList.Set32BitShaderConstants(BlurHorizontalShader.Get(), &SSAOSettings.ScreenSize, 2);

	//Vertical Blur
	commandList.Dispatch(dispatchWidth, dispatchHeight, 1);

	commandList.UnorderedAccessTextureBarrier(frameResources.SSAOBuffer.Get());

	commandList.SetComputePipelineState(BlurVerticalPSO.Get());

	commandList.Set32BitShaderConstants(BlurVerticalShader.Get(), &SSAOSettings.ScreenSize, 2);

	commandList.Dispatch(dispatchWidth, dispatchHeight, 1);

	commandList.UnorderedAccessTextureBarrier(frameResources.SSAOBuffer.Get());

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END SSAO ==")
}

bool CEDXScreenSpaceOcclusionRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
	return CreateRenderTarget(resources);
}

bool CEDXScreenSpaceOcclusionRenderer::CreateRenderTarget(Main::Rendering::CEFrameResources& frameResources) {
	const uint32 width = frameResources.MainWindowViewport->GetWidth();
	const uint32 height = frameResources.MainWindowViewport->GetHeight();
	const uint32 flags = Main::RenderLayer::TextureFlags_RWTexture;

	frameResources.SSAOBuffer = CastGraphicsManager()->CreateTexture2D(frameResources.SSAOBufferFormat,
	                                                                   width,
	                                                                   height,
	                                                                   1,
	                                                                   1,
	                                                                   flags,
	                                                                   RenderLayer::CEResourceState::Common,
	                                                                   nullptr);
	if (!frameResources.SSAOBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	frameResources.SSAOBuffer->SetName("SSAO Buffer");

	return true;
}
