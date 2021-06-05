#include "CEDXSkyBoxRenderPass.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXSkyBoxRenderPass::Create(const Main::Rendering::CEFrameResources& resources,
                                  const Main::Rendering::CEPanoramaConfig& panoramaConfig) {
	SkyboxMesh = CastMeshManager()->CreateSphere(1);

	RenderLayer::CEResourceData vertexData = RenderLayer::CEResourceData(
		SkyboxMesh.Vertices.Data(), SkyboxMesh.Vertices.SizeInBytes());
	SkyboxVertexBuffer = CastGraphicsManager()->CreateVertexBuffer<Main::CEVertex>(
		SkyboxMesh.Vertices.Size(), RenderLayer::BufferFlag_Upload,
		RenderLayer::CEResourceState::VertexAndConstantBuffer, &vertexData);
	if (!SkyboxVertexBuffer) {
		return false;
	}

	SkyboxVertexBuffer->SetName("Skybox Vertex Buffer");

	RenderLayer::CEResourceData indexData = RenderLayer::CEResourceData(SkyboxMesh.Indices.Data(),
	                                                                    SkyboxMesh.Indices.SizeInBytes());
	SkyboxIndexBuffer = CastGraphicsManager()->CreateIndexBuffer(RenderLayer::CEIndexFormat::uint32,
	                                                             SkyboxMesh.Indices.Size(),
	                                                             RenderLayer::BufferFlag_Upload,
	                                                             RenderLayer::CEResourceState::VertexAndConstantBuffer,
	                                                             &indexData);
	if (!SkyboxIndexBuffer) {
		return false;
	}

	SkyboxIndexBuffer->SetName("Skybox Index Buffer");

	if (!panoramaConfig.SourceFile.data()) {
		return false;
	}

	ConceptEngine::Core::Common::CERef<RenderLayer::CETexture2D> panorama =
		Main::Managers::CETextureManager::LoadFromFile(
			panoramaConfig.SourceFile, 0, RenderLayer::CEFormat::R32G32B32A32_Float);
	if (!panorama) {
		return false;
	}

	panorama->SetName(panoramaConfig.SourceFile);

	RenderLayer::CESamplerStateCreateInfo createInfo;
	createInfo.AddressU = RenderLayer::CESamplerMode::Wrap;
	createInfo.AddressV = RenderLayer::CESamplerMode::Wrap;
	createInfo.AddressW = RenderLayer::CESamplerMode::Wrap;
	createInfo.Filter = RenderLayer::CESamplerFilter::MinMagMipLinear;
	createInfo.MinLOD = 0.0f;
	createInfo.MaxLOD = 0.0f;

	SkyboxSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
	if (!SkyboxSampler) {
		return false;
	}

	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/Skybox.hlsl", "VSMain", nullptr,
	                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SkyboxVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!SkyboxVertexShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SkyboxVertexShader->SetName("Skybox Vertex Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/Skybox.hlsl", "PSMain", nullptr, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SkyboxPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!SkyboxPixelShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	SkyboxPixelShader->SetName("skybox Pixel Shader");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	Core::Common::CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(
		rasterizerStateInfo);
	if (!rasterizerState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	rasterizerState->SetName("Skybox Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;
	blendStateInfo.independentBlendEnable = false;
	blendStateInfo.RenderTarget[0].BlendEnable = false;

	Core::Common::CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);
	if (!blendState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	blendState->SetName("Skybox Blend State");

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

	depthStencilState->SetName("Skybox Depth Stencil State");

	CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
	pipelineStateInfo.InputLayoutState = resources.StdInputLayout.Get();
	pipelineStateInfo.BlendState = blendState.Get();
	pipelineStateInfo.DepthStencilState = depthStencilState.Get();
	pipelineStateInfo.RasterizerState = rasterizerState.Get();
	pipelineStateInfo.ShaderState.VertexShader = SkyboxVertexShader.Get();
	pipelineStateInfo.ShaderState.PixelShader = SkyboxPixelShader.Get();
	pipelineStateInfo.PipelineFormats.RenderTargetFormats[0] = resources.FinalTargetFormat;
	pipelineStateInfo.PipelineFormats.NumRenderTargets = 1;
	pipelineStateInfo.PipelineFormats.DepthStencilFormat = resources.DepthBufferFormat;

	PipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(pipelineStateInfo);
	if (!PipelineState) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PipelineState->SetName("Skybox Pipeline State");

	return true;
}

void CEDXSkyBoxRenderPass::Render(Main::RenderLayer::CECommandList& commandList,
                                  const Main::Rendering::CEFrameResources& frameResources,
                                  const Render::Scene::CEScene& scene) {
}
