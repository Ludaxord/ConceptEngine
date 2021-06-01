#include "CEDXRenderer.h"

#include "../../../Core/Application/CECore.h"

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

	Resources.MainWindowViewport = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))
		->CreateViewport(
			Core::Platform::Generic::Callbacks::EngineController.GetWindow(),
			0,
			0,
			CEFormat::R8G8B8A8_Unorm,
			CEFormat::Unknown
		);

	if (!Resources.MainWindowViewport) {
		return false;
	}

	Resources.MainWindowViewport->SetName("Main Window Viewport");

	Resources.CameraBuffer = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateConstantBuffer<CEDXCameraBufferDesc>(
		BufferFlag_Default, CEResourceState::Common, nullptr
	);

	if (!Resources.CameraBuffer) {
		CE_LOG_ERROR("[CERenderer]: Failed to Create Camera Buffer");
		return false;
	}

	Resources.CameraBuffer->SetName("CameraBuffer");

	const auto inputLayout = CreateInputLayoutCreateInfo();

	Resources.StdInputLayout = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager))->CreateInputLayout(inputLayout);
	if (!Resources.StdInputLayout) {
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

		Resources.DirectionalShadowSampler = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))->CreateSamplerState(createInfo);
		if (!Resources.DirectionalShadowSampler) {
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

		Resources.PointShadowSampler = dynamic_cast<Main::Managers::CEGraphicsManager*>(
			Core::Application::CECore::GetGraphics()
			->GetManager(CEManagerType::GraphicsManager))->CreateSamplerState(createInfo);
		if (!Resources.PointShadowSampler) {
			return false;
		}

		Resources.PointShadowSampler->SetName("ShadowMap Comparison Sampler");
	}

	GPUProfiler = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager))->CreateProfiler();
	if (!GPUProfiler) {
		return false;
	}

	Core::Debug::CEProfiler::SetGPUProfiler(GPUProfiler.Get());

	if (!CreateAA()) {
		return false;
	}

	if (!CreateBoundingBoxDebugPass()) {
		return false;
	}

	if (!CreateShadingImage()) {
		return false;
	}

	if (!LightSetup.Create()) {
		return false;
	}

	if (!DeferredRenderer.Create(Resources)) {
		return false;
	}

	if (!ShadowMapRenderer.Create(LightSetup, Resources)) {
		return false;
	}

	if (!SSAORenderer.Create(Resources)) {
		return false;
	}

	if (!LightProbeRenderer.Create(LightSetup, Resources)) {
		return false;
	}

	if (!SkyBoxRenderPass.Create(Resources)) {
		return false;
	}

	if (!ForwardRenderer.Create(Resources)) {
		return false;
	}

	if (dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->IsRayTracingSupported()) {
		if (!RayTracer.Create(Resources)) {
			return false;
		}
	}

	using namespace std::placeholders;
	CommandList.Execute([this] {
		LightProbeRenderer.RenderSkyLightProbe(CommandList, LightSetup, Resources);
	});

	CommandListExecutor.ExecuteCommandList(CommandList);

	Core::Platform::Generic::Callbacks::EngineController.OnWindowResizedEvent.AddObject(
		this, &CEDXRenderer::OnWindowResize);

	return true;
}

void CEDXRenderer::Release() {
	CERenderer::Release();
}

void CEDXRenderer::PerformFrustumCulling(const CEScene& scene) {
	CERenderer::PerformFrustumCulling(scene);

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
	CERenderer::PerformFXAA(commandList);
}

void CEDXRenderer::PerformBackBufferBlit(CECommandList& commandList) {
	CERenderer::PerformBackBufferBlit(commandList);
}

void CEDXRenderer::RenderDebugInterface() {
	CERenderer::RenderDebugInterface();
}

void CEDXRenderer::OnWindowResize(const CEWindowResizeEvent& Event) {
	CERenderer::OnWindowResize(Event);
}

bool CEDXRenderer::CreateBoundingBoxDebugPass() {
	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/Debug.hlsl", "VSMain", nullptr, CEShaderStage::Vertex,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		return false;
	}

	AABBVertexShader = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateVertexShader(shaderCode);
	if (!AABBVertexShader) {
		return false;
	}

	AABBVertexShader->SetName("Debug Vertex Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shader/Debug.hlsl", "PSMain", nullptr, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		return false;
	}

	AABBPixelShader = dynamic_cast<Main::Managers::CEGraphicsManager*>(Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreatePixelShader(shaderCode);
	if (!AABBPixelShader) {
		return false;
	}

	AABBPixelShader->SetName("Debug Pixel Shader");

	CEInputLayoutStateCreateInfo inputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0}
	};

	CERef<CEInputLayoutState> inputLayoutState = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateInputLayout(inputLayout);
	if (!inputLayoutState) {
		return false;
	}

	inputLayoutState->SetName("Debug Input Layout State");

	CEDepthStencilStateCreateInfo depthStencilStateInfo;
	depthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	depthStencilStateInfo.DepthEnable = false;
	depthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::Zero;

	CERef<CEDepthStencilState> depthStencilState = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateDepthStencilState(depthStencilStateInfo);
	if (!depthStencilState) {
		return false;
	}

	depthStencilState->SetName("Debug Depth Stencil State");

	CERasterizerStateCreateInfo rasterizerStateInfo;
	rasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> rasterizerState = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateRasterizerState(rasterizerStateInfo);

	if (!rasterizerState) {
		return false;
	}

	rasterizerState->SetName("Debug Rasterizer State");

	CEBlendStateCreateInfo blendStateInfo;

	CERef<CEBlendState> blendState = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateBlendState(blendStateInfo);

	if (!blendState) {
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

	AABBDebugPipelineState = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateGraphicsPipelineState(psoProperties);

	if (!AABBDebugPipelineState) {
		return false;
	}

	AABBDebugPipelineState->SetName("Debug Pipeline State");

	Core::Containers::CEStaticArray<DirectX::XMFLOAT3, 8> vertices = {
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

	AABBVertexBuffer = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateVertexBuffer<DirectX::XMFLOAT3>(
		vertices.Size(), BufferFlag_Default, CEResourceState::Common, &vertexData);
	if (!AABBVertexBuffer) {
		return false;
	}

	AABBVertexBuffer->SetName("AABB Vertex Buffer");

	Core::Containers::CEStaticArray<uint16, 24> indices = {
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

	AABBIndexBuffer = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->
		GetManager(CEManagerType::GraphicsManager))->CreateIndexBuffer(CEIndexFormat::uint16, indices.Size(),
		                                                               BufferFlag_Default, CEResourceState::Common,
		                                                               &indexData);
	if (!AABBIndexBuffer) {
		return false;
	}

	AABBIndexBuffer->SetName("AABB Index Buffer");

	return false;
}

bool CEDXRenderer::CreateAA() {
	return false;
}

bool CEDXRenderer::CreateShadingImage() {
	return false;
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
}


void CEDXRenderer::ResizeResources(uint32 width, uint32 height) {
	if (!Resources.MainWindowViewport->Resize(width, height)) {
		return;
	}

	if (!DeferredRenderer.ResizeResources(Resources)) {
		return;
	}

	if (!SSAORenderer.ResizeResources(Resources)) {
		return;
	}
}
