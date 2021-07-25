#include "CEDirectXRenderer.h"

#include <imgui.h>

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Core/Platform/Generic/Callbacks/CEEngineController.h"

#include "../../../Graphics/Main/Common/CEMaterial.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../Main/Rendering/CEFrameResources.h"

#include "../Rendering/Base/CEDXBaseDeferredRenderer.h"
#include "../Rendering/Base/CEDXBaseShadowMapRenderer.h"
#include "../Rendering/Base/CEDXBaseScreenSpaceOcclusionRenderer.h"
#include "../Rendering/Base/CEDXBaseLightProbeRenderer.h"
#include "../Rendering/Base/CEDXBaseSkyBoxRenderPass.h"
#include "../Rendering/Base/CEDXBaseForwardRenderer.h"
#include "../Rendering/Base/CEDXBaseRayTracer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Render;
using namespace ConceptEngine::Graphics::Main::RenderLayer;


ConceptEngine::Render::CERenderer* Renderer = new CEDirectXRenderer();

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GDrawTextureDebugger(false);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GDrawRendererInfo(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GEnableSSAO(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GEnableFXAA(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GFXAADebug(false);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GEnableVariableRateShading(false);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GPrePassEnabled(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GDrawAABBs(false);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GVSyncEnabled(false);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GFrustumCullEnabled(true);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx GRayTracingEnabled(false);

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

bool CEDirectXRenderer::Create() {

	CE_LOG_DEBUGX(std::string("Concept Engine DirectX Renderer Lanuched"));

	INIT_CONSOLE_VARIABLE("CE.DrawTextureDebugger", &GDrawTextureDebugger);
	INIT_CONSOLE_VARIABLE("CE.DrawRendererInfo", &GDrawRendererInfo);
	INIT_CONSOLE_VARIABLE("CE.EnableSSAO", &GEnableSSAO);
	INIT_CONSOLE_VARIABLE("CE.EnableFXAA", &GEnableFXAA);
	INIT_CONSOLE_VARIABLE("CE.GFXAADebug", &GFXAADebug);
	INIT_CONSOLE_VARIABLE("CE.EnableVariableRateShading", &GEnableVariableRateShading);
	INIT_CONSOLE_VARIABLE("CE.PrePassEnabled", &GPrePassEnabled);
	INIT_CONSOLE_VARIABLE("CE.DrawAABB", &GDrawAABBs);
	INIT_CONSOLE_VARIABLE("CE.VSyncEnabled", &GVSyncEnabled);
	INIT_CONSOLE_VARIABLE("CE.FrustumCullEnabled", &GFrustumCullEnabled);
	INIT_CONSOLE_VARIABLE("CE.RayTracingEnabled", &GRayTracingEnabled);

	Resources.MainWindowViewport = CastGraphicsManager()->CreateViewport(
		EngineController.GetWindow(), 0, 0, CEFormat::R8G8B8A8_Unorm, CEFormat::Unknown);
	if (!Resources.MainWindowViewport) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		Resources.MainWindowViewport->SetName("Main Window Viewport");
	}

	Resources.CameraBuffer = CastGraphicsManager()->CreateConstantBuffer<CEDXCameraBufferDesc>(
		BufferFlag_Default, CEResourceState::Common, nullptr);
	if (!Resources.CameraBuffer) {
		CE_LOG_ERROR("[Renderer]: Failed to create camerabuffer");
		return false;
	}
	else {
		Resources.CameraBuffer->SetName("CameraBuffer");
	}

	// Init standard inputlayout
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
	else {
		Resources.StdInputLayout->SetName("Standard InputLayoutState");
	}

	{
		CESamplerStateCreateInfo CreateInfo;
		CreateInfo.AddressU = CESamplerMode::Border;
		CreateInfo.AddressV = CESamplerMode::Border;
		CreateInfo.AddressW = CESamplerMode::Border;
		CreateInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		CreateInfo.ComparisonFunc = CEComparisonFunc::LessEqual;
		CreateInfo.BorderColor = Math::CEColorF(1.0f, 1.0f, 1.0f, 1.0f);

		Resources.DirectionalShadowSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
		if (!Resources.DirectionalShadowSampler) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			Resources.DirectionalShadowSampler->SetName("ShadowMap Sampler");
		}
	}

	{
		CESamplerStateCreateInfo CreateInfo;
		CreateInfo.AddressU = CESamplerMode::Wrap;
		CreateInfo.AddressV = CESamplerMode::Wrap;
		CreateInfo.AddressW = CESamplerMode::Wrap;
		CreateInfo.Filter = CESamplerFilter::Comparison_MinMagMipLinear;
		CreateInfo.ComparisonFunc = CEComparisonFunc::LessEqual;

		Resources.PointShadowSampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
		if (!Resources.PointShadowSampler) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			Resources.PointShadowSampler->SetName("ShadowMap Comparison Sampler");
		}
	}

	GPUProfiler = CastGraphicsManager()->CreateProfiler();
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

	if (!BaseLightSetup.Create()) {
		return false;
	}

	//TODO: Implement
	DeferredRenderer = new Rendering::Base::CEDXBaseDeferredRenderer();
	if (!DeferredRenderer->Create(Resources)) {
		return false;
	}

	//TODO: Implement
	ShadowMapRenderer = new Rendering::Base::CEDXBaseShadowMapRenderer();
	if (!ShadowMapRenderer->Create(BaseLightSetup, Resources)) {
		return false;
	}

	//TODO: Implement
	SSAORenderer = new Rendering::Base::CEDXBaseScreenSpaceOcclusionRenderer();
	if (!SSAORenderer->Create(Resources)) {
		return false;
	}

	//TODO: Implement
	LightProbeRenderer = new Rendering::Base::CEDXBaseLightProbeRenderer();
	if (!LightProbeRenderer->Create(BaseLightSetup, Resources)) {
		return false;
	}

	//TODO: Implement
	SkyBoxRenderPass = new Rendering::Base::CEDXBaseSkyBoxRenderPass();
	auto PanoConf = Main::Rendering::CEPanoramaConfig{GetEngineSourceDirectory("Assets/Textures/arches.hdr"), true};
	if (!SkyBoxRenderPass->Create(Resources, PanoConf)) {
		return false;
	}

	//TODO: Implement
	ForwardRenderer = new Rendering::Base::CEDXBaseForwardRenderer();
	if (!ForwardRenderer->Create(Resources)) {
		return false;
	}

	if (CastGraphicsManager()->IsRayTracingSupported()) {
		//TODO: Implement
		RayTracer = new Rendering::Base::CEDXBaseRayTracer();
		if (!RayTracer->Create(Resources)) {
			return false;
		}
	}

	CommandList.Begin();

	LightProbeRenderer->RenderSkyLightProbe(CommandList, BaseLightSetup, Resources);

	CommandList.End();
	CommandListExecutor.ExecuteCommandList(CommandList);

	// Register EventFunc
	EngineController.OnWindowResizedEvent.AddObject(this, &CEDirectXRenderer::OnWindowResize);
	return true;
}

void CEDirectXRenderer::Release() {
	CommandListExecutor.WaitForGPU();

	CommandList.Reset();

	DeferredRenderer->Release();
	ShadowMapRenderer->Release();
	SSAORenderer->Release();
	LightProbeRenderer->Release();
	SkyBoxRenderPass->Release();
	ForwardRenderer->Release();
	RayTracer->Release();

	Resources.Release();
	LightSetup.Release();
	BaseLightSetup.Release();

	AABBVertexBuffer.Reset();
	AABBIndexBuffer.Reset();
	AABBDebugPipelineState.Reset();
	AABBVertexShader.Reset();
	AABBPixelShader.Reset();

	PostPipelineState.Reset();
	PostShader.Reset();
	FXAAPipelineState.Reset();
	FXAAShader.Reset();
	FXAADebugPipelineState.Reset();
	FXAADebugShader.Reset();

	ShadingImage.Reset();
	ShadingRatePipeline.Reset();
	ShadingRateShader.Reset();

	GPUProfiler.Reset();
	Core::Debug::CEProfiler::SetGPUProfiler(nullptr);

	LastFrameNumDrawCalls = 0;
	LastFrameNumDispatchCalls = 0;
	LastFrameNumCommands = 0;
}

void CEDirectXRenderer::Update(const ConceptEngine::Render::Scene::CEScene& Scene) {
	// Perform frustum culling
	Resources.DeferredVisibleCommands.Clear();
	Resources.ForwardVisibleCommands.Clear();
	Resources.DebugTextures.Clear();

	if (!GFrustumCullEnabled.GetBool()) {
		for (const Main::Rendering::CEMeshDrawCommand& Command : Scene.GetMeshDrawCommands()) {
			if (Command.Material->HasAlphaMask()) {
				Resources.ForwardVisibleCommands.EmplaceBack(Command);
			}
			else {
				Resources.DeferredVisibleCommands.EmplaceBack(Command);
			}
		}
	}
	else {
		PerformFrustumCulling(Scene);
	}

	Resources.BackBuffer = Resources.MainWindowViewport->GetBackBuffer();

	CommandList.BeginExternalCapture();
	CommandList.Begin();

	Core::Debug::CEProfiler::BeginGPUFrame(CommandList);

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "--BEGIN FRAME--");

	if (ShadingImage && GEnableVariableRateShading.GetBool()) {
		INSERT_DEBUG_CMDLIST_MARKER(CommandList, "Begin VRS Image");
		CommandList.SetShadingRate(CEShadingRate::VRS_1x1);

		CommandList.TransitionTexture(ShadingImage.Get(), CEResourceState::ShadingRateSource,
		                              CEResourceState::UnorderedAccess);

		CommandList.SetComputePipelineState(ShadingRatePipeline.Get());

		CEUnorderedAccessView* ShadingImageUAV = ShadingImage->GetUnorderedAccessView();
		CommandList.SetUnorderedAccessView(ShadingRateShader.Get(), ShadingImageUAV, 0);

		CommandList.Dispatch(ShadingImage->GetWidth(), ShadingImage->GetHeight(), 1);

		CommandList.TransitionTexture(ShadingImage.Get(), CEResourceState::UnorderedAccess,
		                              CEResourceState::ShadingRateSource);

		CommandList.SetShadingRateImage(ShadingImage.Get());

		INSERT_DEBUG_CMDLIST_MARKER(CommandList, "End VRS Image");
	}
	else if (CastGraphicsManager()->IsShadingRateSupported()) {
		CommandList.SetShadingRate(CEShadingRate::VRS_1x1);
	}

	BaseLightSetup.BeginFrame(CommandList, Scene);

	ShadowMapRenderer->RenderPointLightShadows(CommandList, BaseLightSetup, Scene);
	ShadowMapRenderer->RenderDirectionalLightShadows(CommandList, BaseLightSetup, Scene);

	if (CastGraphicsManager()->IsRayTracingSupported()) {
		GPU_TRACE_SCOPE(CommandList, "Ray Tracing");
		RayTracer->PreRender(CommandList, Resources, Scene);
	}

	// Update camerabuffer
	CEDXCameraBufferDesc CamBuff;
	CamBuff.ViewProjection = Scene.GetCamera()->GetViewProjectionMatrix().Native;
	CamBuff.View = Scene.GetCamera()->GetViewMatrix().Native;
	CamBuff.ViewInv = Scene.GetCamera()->GetViewInverseMatrix().Native;
	CamBuff.Projection = Scene.GetCamera()->GetProjectionMatrix().Native;
	CamBuff.ProjectionInv = Scene.GetCamera()->GetProjectionInverseMatrix().Native;
	CamBuff.ViewProjectionInv = Scene.GetCamera()->GetViewProjectionInverseMatrix().Native;
	CamBuff.Position = Scene.GetCamera()->GetPosition().Native;
	CamBuff.Forward = Scene.GetCamera()->GetForward().Native;
	CamBuff.NearPlane = Scene.GetCamera()->GetNearPlane();
	CamBuff.FarPlane = Scene.GetCamera()->GetFarPlane();
	CamBuff.AspectRatio = Scene.GetCamera()->GetAspectRatio();

	CommandList.TransitionBuffer(Resources.CameraBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);

	CommandList.UpdateBuffer(Resources.CameraBuffer.Get(), 0, sizeof(CEDXCameraBufferDesc), &CamBuff);

	CommandList.TransitionBuffer(Resources.CameraBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_ALBEDO_INDEX].Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_NORMAL_INDEX].Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_MATERIAL_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource,
	                              CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX].Get(),
	                              CEResourceState::NonPixelShaderResource, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::DepthWrite);

	Math::CEColorF BlackClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_ALBEDO_INDEX]->GetRenderTargetView(), BlackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_NORMAL_INDEX]->GetRenderTargetView(), BlackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_MATERIAL_INDEX]->GetRenderTargetView(), BlackClearColor);
	CommandList.ClearRenderTargetView(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetRenderTargetView(),
	                                  BlackClearColor);
	CommandList.ClearDepthStencilView(Resources.GBuffer[BUFFER_DEPTH_INDEX]->GetDepthStencilView(),
	                                  CEDepthStencilF(1.0f, 0));

	if (GPrePassEnabled.GetBool()) {
		DeferredRenderer->RenderPrePass(CommandList, Resources);
	}

	{
		GPU_TRACE_SCOPE(CommandList, "Base Pass");
		DeferredRenderer->RenderBasePass(CommandList, Resources);
	}

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_ALBEDO_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_ALBEDO_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_ALBEDO_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_NORMAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_NORMAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_VIEW_NORMAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_MATERIAL_INDEX].Get(), CEResourceState::RenderTarget,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_MATERIAL_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_MATERIAL_INDEX],
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::DepthWrite,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(Resources.SSAOBuffer.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::UnorderedAccess);

	const Math::CEColorF WhiteColor = {1.0f, 1.0f, 1.0f, 1.0f};
	CommandList.ClearUnorderedAccessView(Resources.SSAOBuffer->GetUnorderedAccessView(), WhiteColor);

	if (GEnableSSAO.GetBool()) {
		SSAORenderer->Render(CommandList, Resources);
	}

	CommandList.TransitionTexture(Resources.SSAOBuffer.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::NonPixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.SSAOBuffer->GetShaderResourceView()),
		Resources.SSAOBuffer,
		CEResourceState::NonPixelShaderResource,
		CEResourceState::NonPixelShaderResource);

	CommandList.TransitionTexture(Resources.FinalTarget.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::UnorderedAccess);
	CommandList.TransitionTexture(Resources.BackBuffer, CEResourceState::Present, CEResourceState::RenderTarget);
	CommandList.TransitionTexture(BaseLightSetup.IrradianceMap.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(BaseLightSetup.SpecularIrradianceMap.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);
	CommandList.TransitionTexture(Resources.IntegrationLUT.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::NonPixelShaderResource);

	{
		GPU_TRACE_SCOPE(CommandList, "Light Pass");
		DeferredRenderer->RenderDeferredTiledLightPass(CommandList, Resources, BaseLightSetup);
	}

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::DepthWrite);
	CommandList.TransitionTexture(Resources.FinalTarget.Get(), CEResourceState::UnorderedAccess,
	                              CEResourceState::RenderTarget);

	SkyBoxRenderPass->Render(CommandList, Resources, Scene);

	CommandList.TransitionTexture(BaseLightSetup.PointLightShadowMaps.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(BaseLightSetup.DirLightShadowMaps.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(BaseLightSetup.DirLightShadowMaps->GetShaderResourceView()),
		BaseLightSetup.DirLightShadowMaps,
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource);

	CommandList.TransitionTexture(BaseLightSetup.IrradianceMap.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(BaseLightSetup.SpecularIrradianceMap.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);
	CommandList.TransitionTexture(Resources.IntegrationLUT.Get(), CEResourceState::NonPixelShaderResource,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.IntegrationLUT->GetShaderResourceView()),
		Resources.IntegrationLUT,
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource);

	{
		GPU_TRACE_SCOPE(CommandList, "Forward Pass");
		ForwardRenderer->Render(CommandList, Resources, BaseLightSetup);
	}

	CommandList.TransitionTexture(Resources.FinalTarget.Get(), CEResourceState::RenderTarget,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.FinalTarget->GetShaderResourceView()),
		Resources.FinalTarget,
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource);

	CommandList.TransitionTexture(Resources.GBuffer[BUFFER_DEPTH_INDEX].Get(), CEResourceState::DepthWrite,
	                              CEResourceState::PixelShaderResource);

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView()),
		Resources.GBuffer[BUFFER_DEPTH_INDEX],
		CEResourceState::PixelShaderResource,
		CEResourceState::PixelShaderResource);

	if (GEnableFXAA.GetBool()) {
		PerformFXAA(CommandList);
	}
	else {
		PerformBackBufferBlit(CommandList);
	}

	if (GDrawAABBs.GetBool()) {
		PerformAABBDebugPass(CommandList);
	}

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "Begin UI Render");

	{
		TRACE_SCOPE("Render UI");

		Main::CEGraphics::GetDebugUI()->DrawUI([]() {
			Renderer->RenderDebugInterface();
		});

		if (CastGraphicsManager()->IsShadingRateSupported()) {
			CommandList.SetShadingRate(CEShadingRate::VRS_1x1);
			CommandList.SetShadingRateImage(nullptr);
		}

		Main::CEGraphics::GetDebugUI()->Render(CommandList);
	}

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "End UI Render");

	CommandList.TransitionTexture(Resources.BackBuffer, CEResourceState::RenderTarget, CEResourceState::Present);

	INSERT_DEBUG_CMDLIST_MARKER(CommandList, "--END FRAME--");

	Core::Debug::CEProfiler::EndGPUFrame(CommandList);

	CommandList.End();
	CommandList.EndExternalCapture();

	LastFrameNumDrawCalls = CommandList.GetNumDrawCalls();
	LastFrameNumDispatchCalls = CommandList.GetNumDispatchCalls();
	LastFrameNumCommands = CommandList.GetNumCommands();

	{
		TRACE_SCOPE("ExecuteCommandList");
		CommandListExecutor.ExecuteCommandList(CommandList);
	}

	{
		TRACE_SCOPE("Present");
		Resources.MainWindowViewport->Present(GVSyncEnabled.GetBool());
	}
}

void CEDirectXRenderer::PerformFrustumCulling(const ConceptEngine::Render::Scene::CEScene& scene) {
	TRACE_SCOPE("Frustum Culling");

	ConceptEngine::Render::Scene::CECamera* Camera = scene.GetCamera();
	ConceptEngine::Render::Scene::CEFrustum CameraFrustum = ConceptEngine::Render::Scene::CEFrustum(
		Camera->GetFarPlane(), Camera->GetViewMatrix(), Camera->GetProjectionMatrix());
	for (const Main::Rendering::CEMeshDrawCommand& Command : scene.GetMeshDrawCommands()) {
		const XMFLOAT4X4& Transform = Command.CurrentActor->GetTransform().GetMatrix().Native;
		XMMATRIX XmTransform = XMMatrixTranspose(XMLoadFloat4x4(&Transform));
		XMVECTOR XmTop = XMVectorSetW(XMLoadFloat3(&Command.Mesh->BoundingBox.Top.Native), 1.0f);
		XMVECTOR XmBottom = XMVectorSetW(XMLoadFloat3(&Command.Mesh->BoundingBox.Bottom.Native), 1.0f);
		XmTop = XMVector4Transform(XmTop, XmTransform);
		XmBottom = XMVector4Transform(XmBottom, XmTransform);

		ConceptEngine::Render::Scene::CEAABB Box;
		XMStoreFloat3(&Box.Top.Native, XmTop);
		XMStoreFloat3(&Box.Bottom.Native, XmBottom);
		if (CameraFrustum.CheckAABB(Box)) {
			if (Command.Material->HasAlphaMask()) {
				Resources.ForwardVisibleCommands.EmplaceBack(Command);
			}
			else {
				Resources.DeferredVisibleCommands.EmplaceBack(Command);
			}
		}
	}
}

void CEDirectXRenderer::PerformFXAA(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin FXAA");

	TRACE_SCOPE("FXAA");

	struct FXAASettings {
		float Width;
		float Height;
	} Settings;

	Settings.Width = static_cast<float>(Resources.BackBuffer->GetWidth());
	Settings.Height = static_cast<float>(Resources.BackBuffer->GetHeight());

	CERenderTargetView* BackBufferRTV = Resources.BackBuffer->GetRenderTargetView();
	commandList.SetRenderTargets(&BackBufferRTV, 1, nullptr);

	CEShaderResourceView* FinalTargetSRV = Resources.FinalTarget->GetShaderResourceView();
	if (GFXAADebug.GetBool()) {
		commandList.SetShaderResourceView(FXAADebugShader.Get(), FinalTargetSRV, 0);
		commandList.SetSamplerState(FXAADebugShader.Get(), Resources.FXAASampler.Get(), 0);
		commandList.Set32BitShaderConstants(FXAADebugShader.Get(), &Settings, 2);
		commandList.SetGraphicsPipelineState(FXAADebugPipelineState.Get());
	}
	else {
		commandList.SetShaderResourceView(FXAAShader.Get(), FinalTargetSRV, 0);
		commandList.SetSamplerState(FXAAShader.Get(), Resources.FXAASampler.Get(), 0);
		commandList.Set32BitShaderConstants(FXAAShader.Get(), &Settings, 2);
		commandList.SetGraphicsPipelineState(FXAAPipelineState.Get());
	}

	commandList.DrawInstanced(3, 1, 0, 0);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End FXAA");
}

void CEDirectXRenderer::PerformBackBufferBlit(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin Draw BackBuffer");

	TRACE_SCOPE("Draw to BackBuffer");

	CERenderTargetView* BackBufferRTV = Resources.BackBuffer->GetRenderTargetView();
	commandList.SetRenderTargets(&BackBufferRTV, 1, nullptr);

	CEShaderResourceView* FinalTargetSRV = Resources.FinalTarget->GetShaderResourceView();
	commandList.SetShaderResourceView(PostShader.Get(), FinalTargetSRV, 0);
	commandList.SetSamplerState(PostShader.Get(), Resources.GBufferSampler.Get(), 0);

	commandList.SetGraphicsPipelineState(PostPipelineState.Get());
	commandList.DrawInstanced(3, 1, 0, 0);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End Draw BackBuffer");
}

void CEDirectXRenderer::PerformAABBDebugPass(CECommandList& commandList) {
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "Begin DebugPass");

	TRACE_SCOPE("DebugPass");

	commandList.SetGraphicsPipelineState(AABBDebugPipelineState.Get());

	commandList.SetPrimitiveTopology(CEPrimitiveTopology::LineList);

	commandList.SetConstantBuffer(AABBVertexShader.Get(), Resources.CameraBuffer.Get(), 0);

	commandList.SetVertexBuffers(&AABBVertexBuffer, 1, 0);
	commandList.SetIndexBuffer(AABBIndexBuffer.Get());

	for (const Main::Rendering::CEMeshDrawCommand& Command : Resources.DeferredVisibleCommands) {
		ConceptEngine::Render::Scene::CEAABB& Box = Command.Mesh->BoundingBox;
		XMFLOAT3 Scale = XMFLOAT3(Box.GetWidth(), Box.GetHeight(), Box.GetDepth());
		XMFLOAT3 Position = Box.GetCenter().Native;

		XMMATRIX XmTranslation = XMMatrixTranslation(Position.x, Position.y, Position.z);
		XMMATRIX XmScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

		XMFLOAT4X4 Transform = Command.CurrentActor->GetTransform().GetMatrix().Native;
		XMMATRIX XmTransform = XMMatrixTranspose(XMLoadFloat4x4(&Transform));
		XMStoreFloat4x4(&Transform, XMMatrixMultiplyTranspose(XMMatrixMultiply(XmScale, XmTranslation), XmTransform));

		commandList.Set32BitShaderConstants(AABBVertexShader.Get(), &Transform, 16);

		commandList.DrawIndexedInstanced(24, 1, 0, 0, 0);
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "End DebugPass");
}

void CEDirectXRenderer::RenderDebugInterface() {
	if (GDrawTextureDebugger.GetBool()) {
		constexpr float InvAspectRatio = 16.0f / 9.0f;
		constexpr float AspectRatio = 9.0f / 16.0f;

		const uint32 WindowWidth = EngineController.GetWindow()->GetWidth();
		const uint32 WindowHeight = EngineController.GetWindow()->GetHeight();
		const float Width = Math::CEMath::Max(WindowWidth * 0.6f, 400.0f);
		const float Height = WindowHeight * 0.75f;

		ImGui::SetNextWindowPos(ImVec2(float(WindowWidth) * 0.5f, float(WindowHeight) * 0.175f), ImGuiCond_Appearing,
		                        ImVec2(0.5f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(Width, Height), ImGuiCond_Appearing);

		const ImGuiWindowFlags Flags =
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoSavedSettings;

		bool TempDrawTextureDebugger = GDrawTextureDebugger.GetBool();
		if (ImGui::Begin("FrameBuffer Debugger", &TempDrawTextureDebugger, Flags)) {
			ImGui::BeginChild("##ScrollBox", ImVec2(Width * 0.985f, Height * 0.125f), true,
			                  ImGuiWindowFlags_HorizontalScrollbar);

			const int32 Count = Resources.DebugTextures.Size();
			static int32 SelectedImage = -1;
			if (SelectedImage >= Count) {
				SelectedImage = -1;
			}

			for (int32 i = 0; i < Count; i++) {
				ImGui::PushID(i);

				constexpr float MenuImageSize = 96.0f;
				int32 FramePadding = 2;
				ImVec2 Size = ImVec2(MenuImageSize * InvAspectRatio, MenuImageSize);
				ImVec2 Uv0 = ImVec2(0.0f, 0.0f);
				ImVec2 Uv1 = ImVec2(1.0f, 1.0f);
				ImVec4 BgCol = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
				ImVec4 TintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

				Main::Rendering::ImGuiImage* CurrImage = &Resources.DebugTextures[i];
				if (ImGui::ImageButton(CurrImage, Size, Uv0, Uv1, FramePadding, BgCol, TintCol)) {
					SelectedImage = i;
				}

				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip("%s", CurrImage->Image->GetName().c_str());
				}

				ImGui::PopID();

				if (i != Count - 1) {
					ImGui::SameLine();
				}
			}

			ImGui::EndChild();

			const float ImageWidth = Width * 0.985f;
			const float ImageHeight = ImageWidth * AspectRatio;
			const int32 ImageIndex = SelectedImage < 0 ? 0 : SelectedImage;
			Main::Rendering::ImGuiImage* CurrImage = &Resources.DebugTextures[ImageIndex];
			ImGui::Image(CurrImage, ImVec2(ImageWidth, ImageHeight));
		}

		ImGui::End();

		GDrawTextureDebugger.SetBool(TempDrawTextureDebugger);
	}

	if (GDrawRendererInfo.GetBool()) {
		const uint32 WindowWidth = EngineController.GetWindow()->GetWidth();
		const uint32 WindowHeight = EngineController.GetWindow()->GetHeight();
		const float Width = 300.0f;
		const float Height = WindowHeight * 0.8f;

		ImGui::SetNextWindowPos(ImVec2(float(WindowWidth), 10.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(Width, Height), ImGuiCond_Always);

		const ImGuiWindowFlags Flags =
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoSavedSettings;

		ImGui::Begin("Renderer Window", nullptr, Flags);

		ImGui::Text("Renderer Status:");
		ImGui::Separator();

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, 100.0f);

		const std::string AdapterName = CastGraphicsManager()->GetAdapterName();
		ImGui::Text("Adapter: ");
		ImGui::NextColumn();

		ImGui::Text("%s", AdapterName.c_str());
		ImGui::NextColumn();

		ImGui::Text("DrawCalls: ");
		ImGui::NextColumn();

		ImGui::Text("%d", LastFrameNumDrawCalls);
		ImGui::NextColumn();

		ImGui::Text("DispatchCalls: ");
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

void CEDirectXRenderer::OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) {
	ResizeResources(Event.Width, Event.Height);
}

bool CEDirectXRenderer::CreateBoundingBoxDebugPass() {
	CEArray<uint8> ShaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Debug.hlsl"), "VSMain", nullptr,
	                                     CEShaderStage::Vertex,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	AABBVertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
	if (!AABBVertexShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		AABBVertexShader->SetName("Debug VertexShader");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Debug.hlsl"), "PSMain", nullptr,
	                                     CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	AABBPixelShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
	if (!AABBPixelShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		AABBPixelShader->SetName("Debug PixelShader");
	}

	CEInputLayoutStateCreateInfo InputLayout =
	{
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
	};

	CERef<CEInputLayoutState> InputLayoutState = CastGraphicsManager()->CreateInputLayout(InputLayout);
	if (!InputLayoutState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		InputLayoutState->SetName("Debug InputLayoutState");
	}

	CEDepthStencilStateCreateInfo DepthStencilStateInfo;
	DepthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
	DepthStencilStateInfo.DepthEnable = false;
	DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::Zero;

	CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->
		CreateDepthStencilState(DepthStencilStateInfo);
	if (!DepthStencilState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		DepthStencilState->SetName("Debug DepthStencilState");
	}

	CERasterizerStateCreateInfo RasterizerStateInfo;
	RasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
	if (!RasterizerState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RasterizerState->SetName("Debug RasterizerState");
	}

	CEBlendStateCreateInfo BlendStateInfo;

	CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
	if (!BlendState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlendState->SetName("Debug BlendState");
	}

	CEGraphicsPipelineStateCreateInfo PSOProperties;
	PSOProperties.BlendState = BlendState.Get();
	PSOProperties.DepthStencilState = DepthStencilState.Get();
	PSOProperties.InputLayoutState = InputLayoutState.Get();
	PSOProperties.RasterizerState = RasterizerState.Get();
	PSOProperties.ShaderState.VertexShader = AABBVertexShader.Get();
	PSOProperties.ShaderState.PixelShader = AABBPixelShader.Get();
	PSOProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Line;
	PSOProperties.PipelineFormats.RenderTargetFormats[0] = Resources.RenderTargetFormat;
	PSOProperties.PipelineFormats.NumRenderTargets = 1;
	PSOProperties.PipelineFormats.DepthStencilFormat = Resources.DepthBufferFormat;

	AABBDebugPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PSOProperties);
	if (!AABBDebugPipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		AABBDebugPipelineState->SetName("Debug PipelineState");
	}

	CEStaticArray<XMFLOAT3, 8> Vertices =
	{
		XMFLOAT3(-0.5f, -0.5f, 0.5f),
		XMFLOAT3(0.5f, -0.5f, 0.5f),
		XMFLOAT3(-0.5f, 0.5f, 0.5f),
		XMFLOAT3(0.5f, 0.5f, 0.5f),

		XMFLOAT3(0.5f, -0.5f, -0.5f),
		XMFLOAT3(-0.5f, -0.5f, -0.5f),
		XMFLOAT3(0.5f, 0.5f, -0.5f),
		XMFLOAT3(-0.5f, 0.5f, -0.5f)
	};

	CEResourceData VertexData(Vertices.Data(), Vertices.SizeInBytes());

	AABBVertexBuffer = CastGraphicsManager()->CreateVertexBuffer<XMFLOAT3>(
		Vertices.Size(), BufferFlag_Default, CEResourceState::Common,
		&VertexData);
	if (!AABBVertexBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		AABBVertexBuffer->SetName("AABB VertexBuffer");
	}

	// Create IndexBuffer
	CEStaticArray<uint16, 24> Indices =
	{
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

	CEResourceData IndexData(Indices.Data(), Indices.SizeInBytes());

	AABBIndexBuffer = CastGraphicsManager()->CreateIndexBuffer(CEIndexFormat::uint16, Indices.Size(),
	                                                           BufferFlag_Default,
	                                                           CEResourceState::Common, &IndexData);
	if (!AABBIndexBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		AABBIndexBuffer->SetName("AABB IndexBuffer");
	}

	return true;
}

bool CEDirectXRenderer::CreateAA() {
	CEArray<uint8> ShaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FullscreenVS.hlsl"), "Main",
	                                     nullptr,
	                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	CERef<CEVertexShader> VShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
	if (!VShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		VShader->SetName("Fullscreen VertexShader");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/PostProcessPS.hlsl"), "Main",
	                                     nullptr,
	                                     CEShaderStage::Pixel, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	PostShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
	if (!PostShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PostShader->SetName("PostProcess PixelShader");
	}

	CEDepthStencilStateCreateInfo DepthStencilStateInfo;
	DepthStencilStateInfo.DepthFunc = CEComparisonFunc::Always;
	DepthStencilStateInfo.DepthEnable = false;
	DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::Zero;

	CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->
		CreateDepthStencilState(DepthStencilStateInfo);
	if (!DepthStencilState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		DepthStencilState->SetName("PostProcess DepthStencilState");
	}

	CERasterizerStateCreateInfo RasterizerStateInfo;
	RasterizerStateInfo.CullMode = CECullMode::None;

	CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
	if (!RasterizerState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RasterizerState->SetName("PostProcess RasterizerState");
	}

	CEBlendStateCreateInfo BlendStateInfo;
	BlendStateInfo.IndependentBlendEnable = false;
	BlendStateInfo.RenderTarget[0].BlendEnable = false;

	CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
	if (!BlendState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		BlendState->SetName("PostProcess BlendState");
	}

	CEGraphicsPipelineStateCreateInfo PSOProperties;
	PSOProperties.InputLayoutState = nullptr;
	PSOProperties.BlendState = BlendState.Get();
	PSOProperties.DepthStencilState = DepthStencilState.Get();
	PSOProperties.RasterizerState = RasterizerState.Get();
	PSOProperties.ShaderState.VertexShader = VShader.Get();
	PSOProperties.ShaderState.PixelShader = PostShader.Get();
	PSOProperties.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;
	PSOProperties.PipelineFormats.RenderTargetFormats[0] = CEFormat::R8G8B8A8_Unorm;
	PSOProperties.PipelineFormats.NumRenderTargets = 1;
	PSOProperties.PipelineFormats.DepthStencilFormat = CEFormat::Unknown;

	PostPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PSOProperties);
	if (!PostPipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PostPipelineState->SetName("PostProcess PipelineState");
	}

	// FXAA
	CESamplerStateCreateInfo CreateInfo;
	CreateInfo.AddressU = CESamplerMode::Clamp;
	CreateInfo.AddressV = CESamplerMode::Clamp;
	CreateInfo.AddressW = CESamplerMode::Clamp;
	CreateInfo.Filter = CESamplerFilter::MinMagMipLinear;

	Resources.FXAASampler = CastGraphicsManager()->CreateSamplerState(CreateInfo);
	if (!Resources.FXAASampler) {
		return false;
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FXAA_PS.hlsl"), "Main", nullptr,
	                                     CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	FXAAShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
	if (!FXAAShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FXAAShader->SetName("FXAA PixelShader");
	}

	PSOProperties.ShaderState.PixelShader = FXAAShader.Get();

	FXAAPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PSOProperties);
	if (!FXAAPipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FXAAPipelineState->SetName("FXAA PipelineState");
	}

	CEArray<CEShaderDefine> Defines =
	{
		CEShaderDefine("ENABLE_DEBUG", "1")
	};

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/FXAA_PS.hlsl"), "Main",
	                                     &Defines, CEShaderStage::Pixel,
	                                     CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	FXAADebugShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
	if (!FXAADebugShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FXAADebugShader->SetName("FXAA PixelShader");
	}

	PSOProperties.ShaderState.PixelShader = FXAADebugShader.Get();

	FXAADebugPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PSOProperties);
	if (!FXAADebugPipelineState) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		FXAADebugPipelineState->SetName("FXAA Debug PipelineState");
	}

	return true;
}

bool CEDirectXRenderer::CreateShadingImage() {
	Main::CEShadingRateSupport Support;
	CastGraphicsManager()->CheckShadingRateSupport(Support);

	if (Support.Tier != Main::CEShadingRateTier::Tier2 || Support.ShadingRateImageTileSize == 0) {
		return true;
	}

	uint32 Width = Resources.MainWindowViewport->GetWidth() / Support.ShadingRateImageTileSize;
	uint32 Height = Resources.MainWindowViewport->GetHeight() / Support.ShadingRateImageTileSize;
	ShadingImage = CastGraphicsManager()->CreateTexture2D(CEFormat::R8_Uint, Width, Height, 1, 1,
	                                                      TextureFlags_RWTexture,
	                                                      CEResourceState::ShadingRateSource, nullptr);
	if (!ShadingImage) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		ShadingImage->SetName("Shading Rate Image");
	}

	CEArray<uint8> ShaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadingImage.hlsl"), "Main",
	                                     nullptr,
	                                     CEShaderStage::Compute, CEShaderModel::SM_6_0, ShaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadingRateShader = CastGraphicsManager()->CreateComputeShader(ShaderCode);
	if (!ShadingRateShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		ShadingRateShader->SetName("ShadingRate Image Shader");
	}

	CEComputePipelineStateCreateInfo CreateInfo(ShadingRateShader.Get());
	ShadingRatePipeline = CastGraphicsManager()->CreateComputePipelineState(CreateInfo);
	if (!ShadingRatePipeline) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		ShadingRatePipeline->SetName("ShadingRate Image Pipeline");
	}

	return true;
}

void CEDirectXRenderer::ResizeResources(uint32 Width, uint32 Height) {
	if (!Resources.MainWindowViewport->Resize(Width, Height)) {
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

const CEInputLayoutStateCreateInfo& CEDirectXRenderer::CreateInputLayoutCreateInfo() {
	const CEInputLayoutStateCreateInfo InputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
		{"NORMAL", 0, CEFormat::R32G32B32_Float, 0, 12, CEInputClassification::Vertex, 0},
		{"TANGENT", 0, CEFormat::R32G32B32_Float, 0, 24, CEInputClassification::Vertex, 0},
		{"TEXCOORD", 0, CEFormat::R32G32_Float, 0, 36, CEInputClassification::Vertex, 0}
	};

	return InputLayout;
}
