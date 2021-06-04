#include "CEDXScreenSpaceOcclusionRenderer.h"

#include <random>

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../../Math/CEFloat.h"

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

	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/SSAO.hlsl", "Main", nullptr,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SSAOShader = CastGraphicsManager()->CreateComputeShader(shaderCode);
	if (!SSAOShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SSAOShader->SetName("SSAO Shader");

	RenderLayer::CEComputePipelineStateCreateInfo pipelineStateCreateInfo;
	pipelineStateCreateInfo.Shader = SSAOShader.Get();

	PipelineState = CastGraphicsManager()->CreateComputePipelineState(pipelineStateCreateInfo);
	if (!PipelineState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PipelineState->SetName("SSAO Pipeline State");

	//TODO: Change all constants digits into editor option modifiers.

	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	Core::Containers::CEArray<DirectX::XMFLOAT3> SSAOKernel;
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
	Core::Containers::CEArray<CEFloat16> SSAONoise;
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
		Core::Debug::CEDebug::DebugBreak();
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
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SSAOSamples->SetName("SSAO Samples");

	//TODO Finish!!
	
	return true;
}

void CEDXScreenSpaceOcclusionRenderer::Render(Main::RenderLayer::CECommandList& commandList,
                                              Main::Rendering::CEFrameResources& frameResources) {
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
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	frameResources.SSAOBuffer->SetName("SSAO Buffer");

	return true;
}
