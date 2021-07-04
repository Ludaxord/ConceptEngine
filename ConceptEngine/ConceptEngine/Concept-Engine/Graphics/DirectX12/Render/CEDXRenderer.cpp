#include "CEDXRenderer.h"

#include <imgui.h>

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Core/Platform/Generic/Callbacks/CEEngineController.h"

#include "../../../Graphics/Main/Common/CEMaterial.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../Main/Rendering/CEFrameResources.h"

#include "../Rendering/CEDXDeferredRenderer.h"
#include "../Rendering/CEDXShadowMapRenderer.h"
#include "../Rendering/CEDXScreenSpaceOcclusionRenderer.h"
#include "../Rendering/CEDXLightProbeRenderer.h"
#include "../Rendering/CEDXSkyBoxRenderPass.h"
#include "../Rendering/CEDXForwardRenderer.h"
#include "../Rendering/CEDXRayTracer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Render;
using namespace ConceptEngine::Render::Scene;
using namespace ConceptEngine::Graphics::Main::RenderLayer;
using namespace ConceptEngine::Core::Common;

ConceptEngine::Render::CERenderer* Renderer = new CEDXRenderer();

struct CEDXCameraBufferDesc {
	DirectX::XMFLOAT4X4 ViewProjection;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 ViewInv;
	DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 ProjectionInv;
	DirectX::XMFLOAT4X4 ViewProjectionInv;
	DirectX::XMFLOAT3 Position;
	float NearPlane;
	DirectX::XMFLOAT3 Forward;
	float FarPlane;
	float AspectRatio;
};

bool CEDXRenderer::Create() {

	for (auto variable : ConceptEngine::Render::Variables) {
		INIT_CONSOLE_VARIABLE(variable.first, &variable.second);
	}

	Resources.MainWindowViewport = CastGraphicsManager()
		->CreateViewport(
			Core::Generic::Platform::CEPlatform::GetWindow(),
			0,
			0,
			CEFormat::R8G8B8A8_Unorm,
			CEFormat::Unknown
		);

	if (!Resources.MainWindowViewport) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create ViewPort");
		CEDebug::DebugBreak();
		return false;
	}

	Resources.MainWindowViewport->SetName("Main Window Viewport");

	Resources.CameraBuffer = CastGraphicsManager()->CreateConstantBuffer<CEDXCameraBufferDesc>(
		BufferFlag_Default, CEResourceState::Common, nullptr
	);

	if (!Resources.CameraBuffer) {
		CE_LOG_ERROR("[CERenderer]: Failed to Create Camera Buffer");
		return false;
	}

	Resources.CameraBuffer->SetName("CameraBuffer");

	CEInputLayoutStateCreateInfo InputLayout =
	{
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
		{"NORMAL", 0, CEFormat::R32G32B32_Float, 0, 12, CEInputClassification::Vertex, 0},
		{"TANGENT", 0, CEFormat::R32G32B32_Float, 0, 24, CEInputClassification::Vertex, 0},
		{"TEXCOORD", 0, CEFormat::R32G32_Float, 0, 36, CEInputClassification::Vertex, 0},
	};

	Resources.StdInputLayout = CastGraphicsManager()->CreateInputLayout(InputLayout);
	if (!Resources.StdInputLayout) {
		CEDebug::DebugBreak();
		return false;
	}

	Resources.StdInputLayout->SetName("Standard InputLayoutState");

	{
		CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = CESamplerMode::Border;
		createInfo.AddressV = CESamplerMode::Border;
		createInfo.AddressW = CESamplerMode::Border;
		createInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		createInfo.ComparisonFunc = CEComparisonFunc::LessEqual;
		createInfo.BorderColor = Math::CEColorF(1.0f, 1.0f, 1.0f, 1.0f);

		Resources.DirectionalShadowSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
		if (!Resources.DirectionalShadowSampler) {
			CEDebug::DebugBreak();
			return false;
		}

		Resources.DirectionalShadowSampler->SetName("ShadowMap Sampler");
	}

	{
		CESamplerStateCreateInfo createInfo;
		createInfo.AddressU = CESamplerMode::Wrap;
		createInfo.AddressV = CESamplerMode::Wrap;
		createInfo.AddressW = CESamplerMode::Wrap;
		createInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		createInfo.ComparisonFunc = CEComparisonFunc::LessEqual;

		Resources.PointShadowSampler = CastGraphicsManager()->CreateSamplerState(createInfo);
		if (!Resources.PointShadowSampler) {
			CEDebug::DebugBreak();
			return false;
		}

		Resources.PointShadowSampler->SetName("ShadowMap Comparison Sampler");
	}

	GPUProfiler = CastGraphicsManager()->CreateProfiler();
	if (!GPUProfiler) {
		CEDebug::DebugBreak();
		return false;
	}

	Core::Debug::CEProfiler::SetGPUProfiler(GPUProfiler.Get());

	if (!CreateAA()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CreateBoundingBoxDebugPass()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!CreateShadingImage()) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!LightSetup.Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	DeferredRenderer = new Rendering::CEDXDeferredRenderer();

	if (!DeferredRenderer) {
		CEDebug::DebugBreak();
		return false;
	}

	//TODO: Fix, D3D12 ERROR: ID3D12PipelineState::<final-release>: CORRUPTION: An ID3D12PipelineState object ('BRDF Integration Gen Pipeline State') is referenced by GPU operations in-flight on Command Queue ('Unnamed ID3D12CommandQueue Object').  It is not safe to final-release objects that may have GPU operations pending.  This can result in application instability.
	if (!DeferredRenderer->Create(Resources)) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadowMapRenderer = new Rendering::CEDXShadowMapRenderer();

	if (!ShadowMapRenderer) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!ShadowMapRenderer->Create(LightSetup, Resources)) {
		CEDebug::DebugBreak();
		return false;
	}

	SSAORenderer = new Rendering::CEDXScreenSpaceOcclusionRenderer();

	if (!SSAORenderer) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!SSAORenderer->Create(Resources)) {
		CEDebug::DebugBreak();
		return false;
	}

	LightProbeRenderer = new Rendering::CEDXLightProbeRenderer();

	if (!LightProbeRenderer) {
		CEDebug::DebugBreak();
		return false;
	}

	if (!LightProbeRenderer->Create(LightSetup, Resources)) {
		CEDebug::DebugBreak();
		return false;
	}

	SkyBoxRenderPass = new Rendering::CEDXSkyBoxRenderPass();

	if (!SkyBoxRenderPass) {
		CEDebug::DebugBreak();
		return false;
	}

	auto panoConf = Main::Rendering::CEPanoramaConfig{GetEngineSourceDirectory("Assets/Textures/arches.hdr"), true};

	if (!SkyBoxRenderPass->Create(Resources, panoConf)) {
		CEDebug::DebugBreak();
		return false;
	}

	ForwardRenderer = new Rendering::CEDXForwardRenderer();

	if (!ForwardRenderer) {
		CEDebug::DebugBreak();
		return false;
	}
	if (!ForwardRenderer->Create(Resources)) {
		CEDebug::DebugBreak();
		return false;
	}

	if (CastGraphicsManager()->IsRayTracingSupported()) {
		RayTracer = new Rendering::CEDXRayTracer();

		if (!RayTracer) {
			CEDebug::DebugBreak();
			return false;
		}
		//Comment TEST TODO: Uncomment when issue is fixed
		if (!RayTracer->Create(Resources)) {
			CEDebug::DebugBreak();
			return false;
		}
	}

	using namespace std::placeholders;
	// CommandList.Execute([this] {
	CommandList.Begin();
	LightProbeRenderer->RenderSkyLightProbe(CommandList, LightSetup, Resources);
	CommandList.End();
	// });

	CommandListExecutor.ExecuteCommandList(CommandList);

	EngineController.OnWindowResizedEvent.AddObject(
		this, &CEDXRenderer::OnWindowResize);

	return true;
}

void CEDXRenderer::Release() {
	CERenderer::Release();
}

void CEDXRenderer::PerformFrustumCulling(const CEScene& scene) {
	TRACE_SCOPE("Frustum Culling");

	CECamera* camera = scene.GetCamera();
	CEFrustum cameraFrustum = CEFrustum(camera->GetFarPlane(), camera->GetViewMatrix(), camera->GetProjectionMatrix());
	for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
		const auto transform = command.CurrentActor->GetTransform().GetMatrix();
		auto transposeTransform = CEMatrixTranspose(CELoadFloat4X4(&transform));

		auto loadFloatBoundingBoxTop = CELoadFloat3(&command.Mesh->BoundingBox.Top);
		auto transposeTop = CEVectorSetW<3, float>(loadFloatBoundingBoxTop, 1.0f);

		auto loadFloatBoundingBoxBottom = CELoadFloat3(&command.Mesh->BoundingBox.Bottom);
		auto transposeBottom = CEVectorSetW<3, float>(loadFloatBoundingBoxBottom, 1.0f);

		transposeTop = CEVector4Transform<3, 4, 4, float>(transposeTop, transposeTransform);
		transposeBottom = CEVector4Transform<3, 4, 4, float>(transposeBottom, transposeTransform);

		CEAABB Box;
		CEStoreFloat3(&Box.Top, transposeTop);
		CEStoreFloat3(&Box.Bottom, transposeBottom);
		if (cameraFrustum.CheckAABB(Box)) {
			if (command.Material->HasAlphaMask()) {
				Resources.ForwardVisibleCommands.EmplaceBack(command);
			}
			else {
				Resources.DeferredVisibleCommands.EmplaceBack(command);
			}
		}
	}
}

void CEDXRenderer::PerformFXAA(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin FXAA");

	TRACE_SCOPE("FXAA");

	struct FXAASettings {
		float Width;
		float Height;
	} Settings;

	Settings.Width = static_cast<float>(Resources.BackBuffer->GetWidth());
	Settings.Height = static_cast<float>(Resources.BackBuffer->GetHeight());

	CERenderTargetView* backBufferRTV = Resources.BackBuffer->GetRenderTargetView();
	commandList.SetRenderTargets(&backBufferRTV, 1, nullptr);

	CEShaderResourceView* finalTargetSRV = Resources.FinalTarget->GetShaderResourceView();
	if (ConceptEngine::Render::Variables["CE.GFXAADebug"].GetBool()) {
		commandList.SetShaderResourceView(FXAADebugShader.Get(), finalTargetSRV, 0);
		commandList.SetSamplerState(FXAADebugShader.Get(), Resources.FXAASampler.Get(), 0);
		commandList.Set32BitShaderConstants(FXAADebugShader.Get(), &Settings, 2);
		commandList.SetGraphicsPipelineState(FXAADebugPipelineState.Get());
	}
	else {
		commandList.SetShaderResourceView(FXAAShader.Get(), finalTargetSRV, 0);
		commandList.SetSamplerState(FXAAShader.Get(), Resources.FXAASampler.Get(), 0);
		commandList.Set32BitShaderConstants(FXAAShader.Get(), &Settings, 2);
		commandList.SetGraphicsPipelineState(FXAAPipelineState.Get());
	}

	//TODO: create property for vertex count per instance, instance count, start vertex location and start instance location.
	commandList.DrawInstanced(3, 1, 0, 0);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End FXAA");
}

void CEDXRenderer::PerformBackBufferBlit(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin Draw Back Buffer");

	TRACE_SCOPE("Draw to Back Buffer");

	CERenderTargetView* backBufferRTV = Resources.BackBuffer->GetRenderTargetView();
	commandList.SetRenderTargets(&backBufferRTV, 1, nullptr);

	CEShaderResourceView* finalTargetSRV = Resources.FinalTarget->GetShaderResourceView();
	commandList.SetShaderResourceView(PostShader.Get(), finalTargetSRV, 0);
	commandList.SetSamplerState(PostShader.Get(), Resources.GBufferSampler.Get(), 0);

	commandList.SetGraphicsPipelineState(PostPipelineState.Get());

	//TODO: create property for vertex count per instance, instance count, start vertex location and start instance location.
	commandList.DrawInstanced(3, 1, 0, 0);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End Draw Back Buffer");
}

void CEDXRenderer::PerformAABBDebugPass(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin Debug Pass");

	TRACE_SCOPE("Debug Pass");

	commandList.SetGraphicsPipelineState(AABBDebugPipelineState.Get());

	commandList.SetPrimitiveTopology(CEPrimitiveTopology::LineList);

	commandList.SetConstantBuffer(AABBVertexShader.Get(), Resources.CameraBuffer.Get(), 0);

	commandList.SetVertexBuffers(&AABBVertexBuffer, 1, 0);
	commandList.SetIndexBuffer(AABBIndexBuffer.Get());

	for (const Main::Rendering::CEMeshDrawCommand& command : Resources.DeferredVisibleCommands) {
		CEAABB& box = command.Mesh->BoundingBox;

		CEVectorFloat3 scale = CEVectorFloat3(box.GetWidth(), box.GetHeight(), box.GetDepth());
		CEVectorFloat3 position = box.GetCenter();

		CEMatrix ceTranslation = CEMatrixTranslation(position.x, position.y, position.z);
		CEMatrix ceScale = CEMatrixScaling(scale.x, scale.y, scale.z);

		CEMatrixFloat4X4 transform = command.CurrentActor->GetTransform().GetMatrix();
		CEMatrix ceTransform = CEMatrixTranspose(
			CELoadFloat4X4(&transform)
		);
		CEStoreFloat4x4(&transform,
		                CEMatrixMultiplyTranspose(
			                CEMatrixMultiply(ceScale, ceTranslation),
			                ceTransform
		                )
		);

		commandList.Set32BitShaderConstants(AABBVertexShader.Get(), &transform, 16);

		commandList.DrawIndexedInstanced(24, 1, 0, 0, 0);
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End Debug Pass");
}

void CEDXRenderer::RenderDebugInterface() {
	if (ConceptEngine::Render::Variables["CE.DrawTextureDebugger"].GetBool()) {
		//TODO: find actual screen aspect ratio <== Use functions from ConceptEngine Framework
		constexpr float inverseAspectRatio = 16.0f / 9.0f;
		constexpr float AspectRatio = 9.0f / 16.0f;

		const uint32 windowWidth = Core::Generic::Platform::CEPlatform::GetWindow()->GetWidth();
		const uint32 windowHeight = Core::Generic::Platform::CEPlatform::GetWindow()->GetHeight();

		const float width = Math::CEMath::Max(windowWidth * 0.6f, 400.0f);
		const float height = windowHeight * 0.75f;

		ImGui::SetNextWindowPos(ImVec2(float(windowWidth) * 0.5f, float(windowHeight) * 0.175f), ImGuiCond_Appearing,
		                        ImVec2(0.5f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);

		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings;

		bool tempDrawTextureDebugger = ConceptEngine::Render::Variables["CE.DrawTextureDebugger"].GetBool();
		if (ImGui::Begin("Frame Buffer Debugger", &tempDrawTextureDebugger, flags)) {
			ImGui::BeginChild("##ScrollBox", ImVec2(width * 0.985f, height * 0.125f), true,
			                  ImGuiWindowFlags_HorizontalScrollbar);

			const int32 count = Resources.DebugTextures.Size();
			static int32 selectedImage = -1;
			if (selectedImage >= count) {
				selectedImage = -1;
			}

			for (int32 i = 0; i < count; i++) {
				ImGui::PushID(i);

				constexpr float menuImageSize = 96.0f;
				int32 framePadding = 2;

				ImVec2 size = ImVec2(menuImageSize * inverseAspectRatio, menuImageSize);
				ImVec2 uv0 = ImVec2(0.0f, 0.0f);
				ImVec2 uv1 = ImVec2(1.0f, 1.0f);

				ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
				ImVec4 tintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

				Main::Rendering::ImGuiImage* currentImage = &Resources.DebugTextures[i];
				if (ImGui::ImageButton(currentImage, size, uv0, uv1, framePadding, bgColor, tintColor)) {
					selectedImage = i;
				}

				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("%s", currentImage->Image->GetName().c_str());;
				}

				ImGui::PopID();

				if (i != count - 1) {
					ImGui::SameLine();
				}
			}

			ImGui::EndChild();

			const float imageWidth = width * 0.985f;
			const float imageHeight = imageWidth * AspectRatio;
			const int32 imageIndex = selectedImage > 0 ? selectedImage : 0;
			Main::Rendering::ImGuiImage* currentImage = &Resources.DebugTextures[imageIndex];
			ImGui::Image(currentImage, ImVec2(imageWidth, imageHeight));
		}

		ImGui::End();

		ConceptEngine::Render::Variables["CE.DrawTextureDebugger"].SetBool(tempDrawTextureDebugger);
	}

	if (ConceptEngine::Render::Variables["CE.DrawTextureDebugger"].GetBool()) {
		const uint32 windowWidth = Core::Generic::Platform::CEPlatform::GetWindow()->GetWidth();
		const uint32 windowHeight = Core::Generic::Platform::CEPlatform::GetWindow()->GetHeight();
		const float width = 300.0f;
		const float height = windowHeight * 0.8f;

		ImGui::SetNextWindowPos(ImVec2(float(windowWidth), 10.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("Concept Engine DirectX 12 Renderer Window", nullptr, flags);

		ImGui::Text("Concept Engine DirectX 12 Renderer Status: ");
		ImGui::Separator();

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 100.0f);

		const std::string adapterName = CastGraphicsManager()->GetAdapterName();

		ImGui::Text("Adapter: ");
		ImGui::NextColumn();

		ImGui::Text("%s", adapterName.c_str());
		ImGui::NextColumn();

		ImGui::Text("Draw Calls: ");
		ImGui::NextColumn();

		ImGui::Text("%d", LastFrameNumDrawCalls);
		ImGui::NextColumn();

		ImGui::Text("Dispatch Calls: ");
		ImGui::NextColumn();

		ImGui::Text("%d", LastFrameNumDispatchCalls);
		ImGui::NextColumn();

		ImGui::Text("Command Count: ");
		ImGui::NextColumn();

		ImGui::Text("%d", LastFrameNumCommands);

		ImGui::Columns(1);

		ImGui::End();
	}
}

void CEDXRenderer::OnWindowResize(const CEWindowResizeEvent& Event) {
	CERenderer::OnWindowResize(Event);
}

bool CEDXRenderer::CreateBoundingBoxDebugPass() {
	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Debug.hlsl"), "VSMain", nullptr,
	                                     CEShaderStage::Vertex,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Compile Debug Vertex Shader");
		return false;
	}

	AABBVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!AABBVertexShader) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create AABBVertexShader");
		return false;
	}

	AABBVertexShader->SetName("Debug Vertex Shader");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Debug.hlsl"), "PSMain", nullptr,
	                                     CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Compile Debug Pixel Shader");
		return false;
	}

	AABBPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!AABBPixelShader) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create AABBPixelShader");
		return false;
	}

	AABBPixelShader->SetName("Debug Pixel Shader");

	CEInputLayoutStateCreateInfo inputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0}
	};

	CERef<CEInputLayoutState> inputLayoutState = CastGraphicsManager()->CreateInputLayout(inputLayout);
	if (!inputLayoutState) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create InputLayout");
		return false;
	}

	inputLayoutState->SetName("Debug Input Layout State");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	depthStencilStateInfo.DepthEnable = false;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::Zero;

	CERef<CEDepthStencilState> depthStencilState = CastGraphicsManager()->
		CreateDepthStencilState(depthStencilStateInfo);
	if (!depthStencilState) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create Depth Stencil State");
		return false;
	}

	depthStencilState->SetName("Debug Depth Stencil State");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(rasterizerStateInfo);

	if (!rasterizerState) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create Rasterizer State");
		return false;
	}

	rasterizerState->SetName("Debug Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;

	CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);

	if (!blendState) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Blend State");
		return false;
	}

	blendState->SetName("Debug Blend State");

	CEGraphicsPipelineStateCreateInfo psoProperties;
	psoProperties.BlendState = blendState.Get();
	psoProperties.DepthStencilState = depthStencilState.Get();
	psoProperties.InputLayoutState = inputLayoutState.Get();
	psoProperties.RasterizerState = rasterizerState.Get();
	psoProperties.ShaderState.VertexShader = AABBVertexShader.Get();
	psoProperties.ShaderState.PixelShader = AABBPixelShader.Get();
	psoProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Line;
	psoProperties.PipelineFormats.RenderTargetFormats[0] = Resources.RenderTargetFormat;
	psoProperties.PipelineFormats.NumRenderTargets = 1;
	psoProperties.PipelineFormats.DepthStencilFormat = Resources.DepthBufferFormat;

	AABBDebugPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(psoProperties);

	if (!AABBDebugPipelineState) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create AABBDebugPipelineState");
		return false;
	}

	AABBDebugPipelineState->SetName("Debug Pipeline State");

	CEStaticArray<DirectX::XMFLOAT3, 8> vertices = {
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
		DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f),
		DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f),

		DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),
		DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),
		DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f),
		DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f)
	};

	CEResourceData vertexData(vertices.Data(), vertices.SizeInBytes());

	AABBVertexBuffer = CastGraphicsManager()->CreateVertexBuffer<DirectX::XMFLOAT3>(
		vertices.Size(), BufferFlag_Default, CEResourceState::Common, &vertexData);
	if (!AABBVertexBuffer) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create AABBVertexBuffer");
		return false;
	}

	AABBVertexBuffer->SetName("AABB Vertex Buffer");

	CEStaticArray<uint16, 24> indices = {
		0, 1,
		1, 3,
		3, 2,
		2, 0,
		1, 4,
		3, 6,
		6, 4,
		4, 5,
		5, 7,
		7, 6,
		0, 5,
		2, 7,
	};

	CEResourceData indexData(indices.Data(), indices.SizeInBytes());

	AABBIndexBuffer = CastGraphicsManager()->CreateIndexBuffer(CEIndexFormat::uint16, indices.Size(),
	                                                           BufferFlag_Default, CEResourceState::Common,
	                                                           &indexData);
	if (!AABBIndexBuffer) {
		CE_LOG_ERROR("[CEDXRenderer::CreateBoundingBoxDebugPass]: Failed to Create AABBIndexBuffer");
		return false;
	}

	AABBIndexBuffer->SetName("AABB Index Buffer");

	return true;
}

bool CEDXRenderer::CreateAA() {
	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FullscreenVS.hlsl"), "Main",
	                                     nullptr, CEShaderStage::Vertex,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Compile FullscreenVS Shader");
		return false;
	}

	CERef<CEVertexShader> vertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
	if (!vertexShader) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Vertex Shader");
		return false;
	}

	vertexShader->SetName("Fullscreen Vertex Shader");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/PostProcessPS.hlsl"), "Main",
	                                     nullptr, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Compile PostProcessPS Shader");
		return false;
	}

	PostShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!PostShader) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Pixel Shader");
		return false;
	}

	PostShader->SetName("Post Process Pixel Shader");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::Always;
	depthStencilStateInfo.DepthEnable = false;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::Zero;

	CERef<CEDepthStencilState> depthStencilState = CastGraphicsManager()->
		CreateDepthStencilState(depthStencilStateInfo);

	if (!depthStencilState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Depth Stencil State");
		return false;
	}

	depthStencilState->SetName("Post Process Depth Stencil State");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> rasterizerState = CastGraphicsManager()->CreateRasterizerState(rasterizerStateInfo);
	if (!rasterizerState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Rasterizer State");
		return false;
	}

	rasterizerState->SetName("Post Process Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;
	blendStateInfo.independentBlendEnable = false;
	blendStateInfo.RenderTarget[0].BlendEnable = false;

	CERef<CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(blendStateInfo);
	if (!blendState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Blend State");
		return false;
	}

	blendState->SetName("Post Process Blend State");

	CEGraphicsPipelineStateCreateInfo psoProperties;
	psoProperties.InputLayoutState = nullptr;
	psoProperties.BlendState = blendState.Get();
	psoProperties.DepthStencilState = depthStencilState.Get();
	psoProperties.RasterizerState = rasterizerState.Get();
	psoProperties.ShaderState.VertexShader = vertexShader.Get();
	psoProperties.ShaderState.PixelShader = PostShader.Get();
	psoProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;
	psoProperties.PipelineFormats.RenderTargetFormats[0] = CEFormat::R8G8B8A8_Unorm;
	psoProperties.PipelineFormats.NumRenderTargets = 1;
	psoProperties.PipelineFormats.DepthStencilFormat = CEFormat::Unknown;

	PostPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(psoProperties);
	if (!PostPipelineState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Graphics Pipeline State");
		return false;
	}

	PostPipelineState->SetName("Post Process Pipeline State");

	//FXAA
	CESamplerStateCreateInfo createInfo;
	createInfo.AddressU = CESamplerMode::Clamp;
	createInfo.AddressV = CESamplerMode::Clamp;
	createInfo.AddressW = CESamplerMode::Clamp;
	createInfo.Filter = CESamplerFilter::MinMagMipLinear;

	Resources.FXAASampler = CastGraphicsManager()->CreateSamplerState(createInfo);
	if (!Resources.FXAASampler) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create FXAA Sampler State");
		return false;
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FXAA_PS.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Compile FXAA_PS Shader");
		return false;
	}

	FXAAShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
	if (!FXAAShader) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Pixel Shader");
		return false;
	}

	FXAAShader->SetName("FXAA Pixel Shader");

	psoProperties.ShaderState.PixelShader = FXAAShader.Get();

	FXAAPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(psoProperties);
	if (!FXAAPipelineState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create FXAA Graphics Pipeline State");
		return false;
	}

	FXAAPipelineState->SetName("FXAA Pipeline State");

	CEArray<CEShaderDefine> defines = {
		CEShaderDefine("ENABLE_DEBUG", "1")
	};

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FXAA_PS.hlsl"), "Main",
	                                     &defines, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Compile FXAA_PS Shader with DEBUG Define");
		return false;
	}

	FXAADebugShader = CastGraphicsManager()->CreatePixelShader(shaderCode);

	if (!FXAADebugShader) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Debug FXAA Pixel Shader");
		return false;
	}

	FXAADebugShader->SetName("FXAA Debug Pixel Shader");

	psoProperties.ShaderState.PixelShader = FXAADebugShader.Get();

	FXAADebugPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(psoProperties);
	if (!FXAADebugPipelineState) {
		CE_LOG_ERROR("[CEDXRenderer]: Failed to Create Debug FXAA Pipeline State");
		return false;
	}

	FXAADebugPipelineState->SetName("FXAA Debug Pipeline State");

	return true;
}

bool CEDXRenderer::CreateShadingImage() {

	Main::CEShadingRateSupport support;

	CastGraphicsManager()->CheckShadingRateSupport(support);

	if (support.Tier != Main::CEShadingRateTier::Tier2 || support.ShadingRateImageTileSize == 0) {
		return true;
	}

	uint32 width = Resources.MainWindowViewport->GetWidth() / support.ShadingRateImageTileSize;
	uint32 height = Resources.MainWindowViewport->GetHeight() / support.ShadingRateImageTileSize;
	ShadingImage = CastGraphicsManager()->CreateTexture2D(CEFormat::R8_Uint, width, height, 1, 1,
	                                                      TextureFlags_RWTexture,
	                                                      CEResourceState::ShadingRateSource, nullptr);
	if (!ShadingImage) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadingImage->SetName("Shading Rate Image");

	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadingImage.hlsl"), "Main",
	                                     nullptr, CEShaderStage::Compute,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadingRateShader = CastGraphicsManager()->CreateComputeShader(shaderCode);

	if (!ShadingRateShader) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadingRateShader->SetName("Shading Rate Image Shader");

	CEComputePipelineStateCreateInfo createInfo(ShadingRateShader.Get());
	ShadingRatePipeline = CastGraphicsManager()->CreateComputePipelineState(createInfo);
	if (!ShadingRatePipeline) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadingRatePipeline->SetName("Shading Rate Image Pipeline");

	return true;
}

const CEInputLayoutStateCreateInfo& CEDXRenderer::CreateInputLayoutCreateInfo() {
	const CEInputLayoutStateCreateInfo InputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
		{"NORMAL", 0, CEFormat::R32G32B32_Float, 0, 12, CEInputClassification::Vertex, 0},
		{"TANGENT", 0, CEFormat::R32G32B32_Float, 0, 24, CEInputClassification::Vertex, 0},
		{"TEXCOORD", 0, CEFormat::R32G32_Float, 0, 36, CEInputClassification::Vertex, 0}
	};

	return InputLayout;
}


void CEDXRenderer::Update(const CEScene& scene) {
	Resources.DeferredVisibleCommands.Clear();
	Resources.ForwardVisibleCommands.Clear();
	Resources.DebugTextures.Clear();

	if (!ConceptEngine::Render::Variables["CE.FrustumCullEnabled"].GetBool()) {
		for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
			if (command.Material->HasAlphaMask()) {
				Resources.ForwardVisibleCommands.EmplaceBack(command);
			}
			else {
				Resources.DeferredVisibleCommands.EmplaceBack(command);
			}
		}
	}
	else {
		PerformFrustumCulling(scene);
	}


	Resources.BackBuffer = Resources.MainWindowViewport->GetBackBuffer();

	CommandList.BeginExternalCapture();
	// CommandList.Execute([this, &scene] {
	CommandList.Begin();
	Core::Debug::CEProfiler::BeginGPUFrame(CommandList);

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "-- BEGIN FRAME --");

	if (ShadingImage && ConceptEngine::Render::Variables["CE.EnableVariableRateShading"].GetBool()) {
		INSERT_DEBUG_CMDLIST_MARKER(CommandList, "== BEGIN VARIABLE RATE SHADING IMAGE ==");
		CommandList.SetShadingRate(CEShadingRate::VRS_1x1);

		CommandList.TransitionTexture(ShadingImage.Get(), CEResourceState::ShadingRateSource,
		                              CEResourceState::UnorderedAccess);

		CommandList.SetComputePipelineState(ShadingRatePipeline.Get());

		CEUnorderedAccessView* shadingImageUAV = ShadingImage->GetUnorderedAccessView();
		CommandList.SetUnorderedAccessView(ShadingRateShader.Get(), shadingImageUAV, 0);

		CommandList.Dispatch(ShadingImage->GetWidth(), ShadingImage->GetHeight(), 1);

		CommandList.TransitionTexture(ShadingImage.Get(), CEResourceState::UnorderedAccess,
		                              CEResourceState::ShadingRateSource);

		CommandList.SetShadingRateImage(ShadingImage.Get());

		INSERT_DEBUG_CMDLIST_MARKER(CommandList, "== END VARIABLE RATE SHADING IMAGE ==");
	}
	else if (CastGraphicsManager()->IsShadingRateSupported()) {
		CommandList.SetShadingRate(CEShadingRate::VRS_1x1);
	}

	LightSetup.BeginFrame(CommandList, scene);

	ShadowMapRenderer->RenderPointLightShadows(CommandList, LightSetup, scene);
	ShadowMapRenderer->RenderDirectionalLightShadows(CommandList, LightSetup, scene);

	if (CastGraphicsManager()->IsRayTracingSupported() && ConceptEngine::Render::Variables[
		"CE.RayTracingEnabled"].GetBool()) {
		GPU_TRACE_SCOPE(CommandList, "== RAY TRACING ==");
		RayTracer->PreRender(CommandList, Resources, scene);
	}

	CEDXCameraBufferDesc cameraBuffer;
	cameraBuffer.ViewProjection = scene.GetCamera()->GetViewProjectionMatrix().Native;
	cameraBuffer.View = scene.GetCamera()->GetViewMatrix().Native;
	cameraBuffer.ViewInv = scene.GetCamera()->GetViewInverseMatrix().Native;
	cameraBuffer.Projection = scene.GetCamera()->GetProjectionMatrix().Native;
	cameraBuffer.ProjectionInv = scene.GetCamera()->GetProjectionInverseMatrix().Native;
	cameraBuffer.ViewProjectionInv = scene.GetCamera()->GetViewProjectionInverseMatrix().Native;
	cameraBuffer.Position = scene.GetCamera()->GetPosition().Native;
	cameraBuffer.Forward = scene.GetCamera()->GetForward().Native;
	cameraBuffer.NearPlane = scene.GetCamera()->GetNearPlane();
	cameraBuffer.FarPlane = scene.GetCamera()->GetFarPlane();
	cameraBuffer.AspectRatio = scene.GetCamera()->GetAspectRatio();

	//Transition CameraBuffer
	CommandList.TransitionBuffer(Resources.CameraBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);
	CommandList.UpdateBuffer(Resources.CameraBuffer.Get(), 0, sizeof(CEDXCameraBufferDesc), &cameraBuffer);
	CommandList.TransitionBuffer(Resources.CameraBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);

	//
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_ALBEDO_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_NORMAL_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_MATERIAL_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(),
	                              CEResourceState::PixelShaderResource, CEResourceState::DepthWrite);

	Math::CEColorF blackClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_ALBEDO_INDEX]->GetRenderTargetView(),
	                                  blackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_NORMAL_INDEX]->GetRenderTargetView(),
	                                  blackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_MATERIAL_INDEX]->GetRenderTargetView(),
	                                  blackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetRenderTargetView(),
	                                  blackClearColor);
	CommandList.ClearDepthStencilView(Resources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView(),
	                                  CEDepthStencilF(1.0f, 0));

	if (ConceptEngine::Render::Variables["CE.PrePassEnabled"].GetBool()) {
		DeferredRenderer->RenderPrePass(CommandList, Resources);
	}

	{
		GPU_TRACE_SCOPE(CommandList, "== BASE PASS ==");
		DeferredRenderer->RenderPrePass(CommandList, Resources);
	}

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_ALBEDO_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_ALBEDO_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_ALBEDO_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource
	);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_NORMAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_NORMAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource
	);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource
	);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_MATERIAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_MATERIAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_MATERIAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource
	);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::DepthWrite,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(Resources.SSAOBuffer.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::UnorderedAccess);

	const Math::CEColorF whiteColor = {1.0f, 1.0f, 1.0f, 1.0f};
	CommandList.ClearUnorderedAccessView(Resources.SSAOBuffer->GetUnorderedAccessView(), whiteColor);

	if (ConceptEngine::Render::Variables["CE.EnableSSAO"].GetBool()) {
		SSAORenderer->Render(CommandList, Resources);
	}

	CommandList.TransitionTexture(Resources.SSAOBuffer.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.SSAOBuffer->GetShaderResourceView()), Resources.SSAOBuffer,
		CEResourceState::NonPixelShaderResource, CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.FinalTarget.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::UnorderedAccess);
	CommandList.TransitionTexture(Resources.BackBuffer, CEResourceState::Present, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(LightSetup.IrradianceMap.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(LightSetup.SpecularIrradianceMap.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(Resources.IntegrationLUT.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);

	{
		GPU_TRACE_SCOPE(CommandList, "== LIGHT PASS ==");
		DeferredRenderer->RenderDeferredTiledLightPass(CommandList, Resources, LightSetup);
	}

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::DepthWrite);
	CommandList.TransitionTexture(Resources.FinalTarget.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::RenderTarget);

	SkyBoxRenderPass->Render(CommandList, Resources, scene);

	CommandList.TransitionTexture(LightSetup.PointLightShadowMaps.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(LightSetup.DirLightShadowMaps.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(LightSetup.DirLightShadowMaps->GetShaderResourceView()),
		LightSetup.DirLightShadowMaps,
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource
	);

	CommandList.TransitionTexture(LightSetup.IrradianceMap.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(LightSetup.SpecularIrradianceMap.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(Resources.IntegrationLUT.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.IntegrationLUT->GetShaderResourceView()),
		Resources.IntegrationLUT,
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource
	);

	{
		GPU_TRACE_SCOPE(CommandList, "== FORWARD PASS ==");
		ForwardRenderer->Render(CommandList, Resources, LightSetup);
	}

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::DepthWrite,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_DEPTH_INDEX],
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource
	);

	if (ConceptEngine::Render::Variables["CE.EnableFXAA"].GetBool()) {
		PerformFXAA(CommandList);
	}
	else {
		PerformBackBufferBlit(CommandList);
	}

	if (ConceptEngine::Render::Variables["CE.DrawAABB"].GetBool()) {
		PerformAABBDebugPass(CommandList);
	}

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "== BEGIN UI RENDER ==");

	{
		TRACE_SCOPE("== RENDER UI ==");

		Main::CEGraphics::GetDebugUI()->DrawUI([]() {
			Renderer->RenderDebugInterface();
		});

		if (CastGraphicsManager()->IsShadingRateSupported()) {
			CommandList.SetShadingRate(CEShadingRate::VRS_1x1);
			CommandList.SetShadingRateImage(nullptr);
		}

		Main::CEGraphics::GetDebugUI()->Render(CommandList);
	}

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "== END UI RENDER ==");

	CommandList.TransitionTexture(Resources.BackBuffer, CEResourceState::RenderTarget, CEResourceState::Present);

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "-- END FRAME --");

	Core::Debug::CEProfiler::EndGPUFrame(CommandList);
	// });
	CommandList.End();
	CommandList.EndExternalCapture();

	LastFrameNumDrawCalls = CommandList.GetNumDrawCalls();
	LastFrameNumDispatchCalls = CommandList.GetNumDispatchCalls();
	LastFrameNumCommands = CommandList.GetNumCommands();

	{
		TRACE_SCOPE("Execute Command List");
		CommandListExecutor.ExecuteCommandList(CommandList);
	}

	{
		TRACE_SCOPE("Present");
		Resources.MainWindowViewport->Present(ConceptEngine::Render::Variables["CE.VSyncEnabled"].GetBool());
	}
}


void CEDXRenderer::ResizeResources(uint32 width, uint32 height) {
	if (!Resources.MainWindowViewport->Resize(width, height)) {
		CEDebug::DebugBreak();
		return;
	}

	if (!DeferredRenderer->ResizeResources(Resources)) {
		CEDebug::DebugBreak();
		return;
	}

	if (!SSAORenderer->ResizeResources(Resources)) {
		CEDebug::DebugBreak();
		return;
	}
}
