#include "CEDXSkyBoxRenderPass.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../../Render/Scene/CEScene.h"
#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXSkyBoxRenderPass::Create(Main::Rendering::CEFrameResources& resources,
                                  const Main::Rendering::CEPanoramaConfig& panoramaConfig) {
	SkyboxMesh = CastMeshManager()->CreateSphere(1);

	CEResourceData vertexData = CEResourceData(SkyboxMesh.Vertices.Data(), SkyboxMesh.Vertices.SizeInBytes());
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

	CERef<RenderLayer::CETexture2D> panorama =
		Main::Managers::CETextureManager::LoadFromFile(
			panoramaConfig.SourceFile, 0, RenderLayer::CEFormat::R32G32B32A32_Float);
	if (!panorama) {
		return false;
	}

	panorama->SetName(panoramaConfig.SourceFile);

	resources.Skybox = Main::Managers::CETextureManager::CreateTextureCubeFromPanorama(
		panorama.Get(), 1024, Main::Managers::TextureFlag_GenerateMips, CEFormat::R16G16B16A16_Float);
	if (!resources.Skybox) {
		return false;
	}

	resources.Skybox->SetName("Skybox");

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

	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Skybox.hlsl"), "VSMain", nullptr,
	                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
	                                     shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	SkyboxVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!SkyboxVertexShader) {
		CEDebug::DebugBreak();
		return false;
	}

	SkyboxVertexShader->SetName("Skybox Vertex Shader");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Skybox.hlsl"), "PSMain", nullptr, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	SkyboxPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!SkyboxPixelShader) {
		CEDebug::DebugBreak();
		return false;
	}

	SkyboxPixelShader->SetName("skybox Pixel Shader");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(
		rasterizerStateInfo);
	if (!rasterizerState) {
		CEDebug::DebugBreak();
		return false;
	}

	rasterizerState->SetName("Skybox Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;
	blendStateInfo.independentBlendEnable = false;
	blendStateInfo.RenderTarget[0].BlendEnable = false;

	CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);
	if (!blendState) {
		CEDebug::DebugBreak();
		return false;
	}

	blendState->SetName("Skybox Blend State");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	depthStencilStateInfo.DepthEnable = true;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

	CERef<CEDepthStencilState> depthStencilState = CastGraphicsManager()->CreateDepthStencilState(
		depthStencilStateInfo);
	if (!depthStencilState) {
		CEDebug::DebugBreak();
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
		CEDebug::DebugBreak();
		return false;
	}

	PipelineState->SetName("Skybox Pipeline State");

	return true;
}

void CEDXSkyBoxRenderPass::Render(Main::RenderLayer::CECommandList& commandList,
                                  const Main::Rendering::CEFrameResources& frameResources,
                                  const Render::Scene::CEScene& scene) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN SKYBOX ==");

	TRACE_SCOPE("Render Skybox");

	const float renderWidth = float(frameResources.FinalTarget->GetWidth());
	const float renderHeight = float(frameResources.FinalTarget->GetHeight());

	CERenderTargetView* renderTarget[] = {frameResources.FinalTarget->GetRenderTargetView()};
	commandList.SetRenderTargets(renderTarget, 1, nullptr);

	commandList.SetViewport(renderWidth, renderHeight, 0.0f, 1.0f, 0.0f, 0.0f);
	commandList.SetScissorRect(renderWidth, renderHeight, 0, 0);

	commandList.SetRenderTargets(renderTarget, 1, frameResources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView());

	commandList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);
	commandList.SetVertexBuffers(&SkyboxVertexBuffer, 1, 0);
	commandList.SetIndexBuffer(SkyboxIndexBuffer.Get());
	commandList.SetGraphicsPipelineState(PipelineState.Get());

	struct SimpleCameraBuffer {
		DirectX::XMFLOAT4X4 Matrix;
	} SimpleCamera;
	SimpleCamera.Matrix = scene.GetCamera()->GetViewProjectionWithoutTranslateMatrix().Native;

	commandList.Set32BitShaderConstants(SkyboxVertexShader.Get(), &SimpleCamera, 16);

	CEShaderResourceView* skyboxSRV = frameResources.Skybox->GetShaderResourceView();
	commandList.SetShaderResourceView(SkyboxPixelShader.Get(), skyboxSRV, 0);

	commandList.SetSamplerState(SkyboxPixelShader.Get(), SkyboxSampler.Get(), 0);

	commandList.DrawIndexedInstanced(static_cast<uint32>(SkyboxMesh.Indices.Size()), 1, 0, 0, 0);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END SKYBOX ==");
}
