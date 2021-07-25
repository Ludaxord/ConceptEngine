#include "CEDXBaseShadowMapRenderer.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"
#include "../../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"

#include "../../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../../Render/Scene/CEFrustum.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

struct PerShadowMap {
	XMFLOAT4X4 Matrix;
	XMFLOAT3 Position;
	float FarPlane;
};

bool CEDXBaseShadowMapRenderer::Create(
	Main::Rendering::CELightSetup& lightSetup, Main::Rendering::CEFrameResources& resources) {
	return false;
}

void CEDXBaseShadowMapRenderer::RenderPointLightShadows(
	Main::RenderLayer::CECommandList& commandList, const Main::Rendering::CELightSetup& lightSetup,
	const Render::Scene::CEScene& scene) {
}

void CEDXBaseShadowMapRenderer::RenderDirectionalLightShadows(
	Main::RenderLayer::CECommandList& commandList, const Main::Rendering::CELightSetup& lightSetup,
	const Render::Scene::CEScene& scene) {
}

bool CEDXBaseShadowMapRenderer::CreateShadowMaps(Main::Rendering::CELightSetup& frameResources) {
	return false;
}

bool CEDXBaseShadowMapRenderer::CreateShadowMaps(Main::Rendering::CEBaseLightSetup& LightSetup) {
	LightSetup.PointLightShadowMaps = CastGraphicsManager()->CreateTextureCubeArray(
		LightSetup.ShadowMapFormat,
		LightSetup.PointLightShadowSize,
		1, LightSetup.MaxPointLightShadows,
		TextureFlags_ShadowMap,
		CEResourceState::PixelShaderResource,
		nullptr);
	if (LightSetup.PointLightShadowMaps) {
		LightSetup.PointLightShadowMaps->SetName("PointLight ShadowMaps");

		LightSetup.PointLightShadowMapDSVs.Resize(LightSetup.MaxPointLightShadows);
		for (uint32 i = 0; i < LightSetup.MaxPointLightShadows; i++) {
			for (uint32 Face = 0; Face < 6; Face++) {
				auto n = std::string("");
				CEStaticArray<CERef<CEDepthStencilView>, 6>& DepthCube = LightSetup.PointLightShadowMapDSVs[i];
				std::string Name = "ShadowMapDepthStencilCubeMap Face " + std::to_string(Face) + " Index " +
					std::to_string(i);

				//D3D12 ERROR: ID3D12Device::CreateDepthStencilView: The Dimensions of the View are invalid due to at least one of the following conditions. MipSlice (value = 1) must be between 0 and MipLevels-1 of the Texture Resource, 0, inclusively. FirstArraySlice (value = 0) must be between 0 and ArraySize-1 of the Texture Resource, 47, inclusively. With the current FirstArraySlice, ArraySize (value = 1) must be between 1 and 48, inclusively, or -1 to default to all slices from FirstArraySlice, in order that the View fit on the Texture. [ STATE_CREATION ERROR #48: CREATEDEPTHSTENCILVIEW_INVALIDDIMENSIONS]

				DepthCube[Face] = CastGraphicsManager()->CreateDepthStencilViewForTextureCubeArray(
					LightSetup.PointLightShadowMaps.Get(),
					LightSetup.ShadowMapFormat,
					0,
					i,
					GetCubeFaceFromIndex(Face),
					Name);
				if (!DepthCube[Face]) {
					CEDebug::DebugBreak();
					return false;
				}
			}
		}
	}
	else {
		return false;
	}

	LightSetup.DirLightShadowMaps = CastGraphicsManager()->CreateTexture2D(
		LightSetup.ShadowMapFormat,
		LightSetup.ShadowMapWidth,
		LightSetup.ShadowMapHeight,
		1, 1, TextureFlags_ShadowMap,
		CEResourceState::PixelShaderResource,
		nullptr);
	if (LightSetup.DirLightShadowMaps) {
		LightSetup.DirLightShadowMaps->SetName("Directional Light ShadowMaps");
	}
	else {
		return false;
	}

	return true;
}

bool CEDXBaseShadowMapRenderer::Create(
	Main::Rendering::CEBaseLightSetup& LightSetup, Main::Rendering::CEFrameResources& FrameResources) {
	if (!CreateShadowMaps(LightSetup)) {
		return false;
	}

	PerShadowMapBuffer = CastGraphicsManager()->CreateConstantBuffer<PerShadowMap>(
		BufferFlag_Default, CEResourceState::VertexAndConstantBuffer,
		nullptr);
	if (!PerShadowMapBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PerShadowMapBuffer->SetName("PerShadowMap Buffer");
	}

	// Linear Shadow Maps
	CEArray<uint8> ShaderCode;
	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "VSMain",
		                                     nullptr,
		                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
		if (!PointLightVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PointLightVertexShader->SetName("Linear ShadowMap VertexShader");
		}

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "PSMain",
		                                     nullptr,
		                                     CEShaderStage::Pixel, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader = CastGraphicsManager()->CreatePixelShader(ShaderCode);
		if (!PointLightPixelShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PointLightPixelShader->SetName("Linear ShadowMap PixelShader");
		}

		CEDepthStencilStateCreateInfo DepthStencilStateInfo;
		DepthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
		DepthStencilStateInfo.DepthEnable = true;
		DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

		CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->CreateDepthStencilState(
			DepthStencilStateInfo);
		if (!DepthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			DepthStencilState->SetName("Shadow DepthStencilState");
		}

		CERasterizerStateCreateInfo RasterizerStateInfo;
		RasterizerStateInfo.CullMode = CECullMode::Back;

		CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
		if (!RasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			RasterizerState->SetName("Shadow RasterizerState");
		}

		CEBlendStateCreateInfo BlendStateInfo;

		CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
		if (!BlendState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BlendState->SetName("Shadow BlendState");
		}

		CEGraphicsPipelineStateCreateInfo PipelineStateInfo;
		PipelineStateInfo.BlendState = BlendState.Get();
		PipelineStateInfo.DepthStencilState = DepthStencilState.Get();
		PipelineStateInfo.IBStripCutValue = CEIndexBufferStripCutValue::Disabled;
		PipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		PipelineStateInfo.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;
		PipelineStateInfo.RasterizerState = RasterizerState.Get();
		PipelineStateInfo.SampleCount = 1;
		PipelineStateInfo.SampleQuality = 0;
		PipelineStateInfo.SampleMask = 0xffffffff;
		PipelineStateInfo.ShaderState.VertexShader = PointLightVertexShader.Get();
		PipelineStateInfo.ShaderState.PixelShader = PointLightPixelShader.Get();
		PipelineStateInfo.PipelineFormats.NumRenderTargets = 0;
		PipelineStateInfo.PipelineFormats.DepthStencilFormat = LightSetup.ShadowMapFormat;

		PointLightPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PipelineStateInfo);
		if (!PointLightPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			PointLightPipelineState->SetName("Linear ShadowMap PipelineState");
		}
	}

	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "Main",
		                                     nullptr,
		                                     CEShaderStage::Vertex, CEShaderModel::SM_6_0, ShaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		DirLightShader = CastGraphicsManager()->CreateVertexShader(ShaderCode);
		if (!DirLightShader) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			DirLightShader->SetName("ShadowMap VertexShader");
		}

		CEDepthStencilStateCreateInfo DepthStencilStateInfo;
		DepthStencilStateInfo.DepthFunc = CEComparisonFunc::LessEqual;
		DepthStencilStateInfo.DepthEnable = true;
		DepthStencilStateInfo.DepthWriteMask = CEDepthWriteMask::All;

		CERef<CEDepthStencilState> DepthStencilState = CastGraphicsManager()->CreateDepthStencilState(
			DepthStencilStateInfo);
		if (!DepthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			DepthStencilState->SetName("Shadow DepthStencilState");
		}

		CERasterizerStateCreateInfo RasterizerStateInfo;
		RasterizerStateInfo.CullMode = CECullMode::Back;

		CERef<CERasterizerState> RasterizerState = CastGraphicsManager()->CreateRasterizerState(RasterizerStateInfo);
		if (!RasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			RasterizerState->SetName("Shadow RasterizerState");
		}

		CEBlendStateCreateInfo BlendStateInfo;
		CERef<CEBlendState> BlendState = CastGraphicsManager()->CreateBlendState(BlendStateInfo);
		if (!BlendState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			BlendState->SetName("Shadow BlendState");
		}

		CEGraphicsPipelineStateCreateInfo PipelineStateInfo;
		PipelineStateInfo.BlendState = BlendState.Get();
		PipelineStateInfo.DepthStencilState = DepthStencilState.Get();
		PipelineStateInfo.IBStripCutValue = CEIndexBufferStripCutValue::Disabled;
		PipelineStateInfo.InputLayoutState = FrameResources.StdInputLayout.Get();
		PipelineStateInfo.PrimitiveTopologyType = CEPrimitiveTopologyType::Triangle;
		PipelineStateInfo.RasterizerState = RasterizerState.Get();
		PipelineStateInfo.SampleCount = 1;
		PipelineStateInfo.SampleQuality = 0;
		PipelineStateInfo.SampleMask = 0xffffffff;
		PipelineStateInfo.ShaderState.VertexShader = DirLightShader.Get();
		PipelineStateInfo.ShaderState.PixelShader = nullptr;
		PipelineStateInfo.PipelineFormats.NumRenderTargets = 0;
		PipelineStateInfo.PipelineFormats.DepthStencilFormat = LightSetup.ShadowMapFormat;

		DirLightPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(PipelineStateInfo);
		if (!DirLightPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}
		else {
			DirLightPipelineState->SetName("ShadowMap PipelineState");
		}
	}

	return true;
}

void CEDXBaseShadowMapRenderer::RenderPointLightShadows(
	Main::RenderLayer::CECommandList& CmdList, const Main::Rendering::CEBaseLightSetup& LightSetup,
	const Render::Scene::CEScene& Scene) {
	PointLightFrame++;
	if (PointLightFrame > 6) {
		UpdatePointLight = true;
		PointLightFrame = 0;
	}

	CmdList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);

	CmdList.TransitionTexture(LightSetup.PointLightShadowMaps.Get(), CEResourceState::PixelShaderResource,
	                          CEResourceState::DepthWrite);

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin Render PointLight ShadowMaps");

	if (UpdatePointLight) {
		TRACE_SCOPE("Render PointLight ShadowMaps");

		const uint32 PointLightShadowSize = LightSetup.PointLightShadowSize;
		CmdList.SetViewport(static_cast<float>(PointLightShadowSize), static_cast<float>(PointLightShadowSize), 0.0f,
		                    1.0f, 0.0f, 0.0f);
		CmdList.SetScissorRect(static_cast<float>(PointLightShadowSize), static_cast<float>(PointLightShadowSize), 0,
		                       0);

		CmdList.SetGraphicsPipelineState(PointLightPipelineState.Get());

		// PerObject Structs
		struct ShadowPerObject {
			XMFLOAT4X4 Matrix;
			float ShadowOffset;
		} ShadowPerObjectBuffer;

		PerShadowMap PerShadowMapData;
		for (uint32 i = 0; i < LightSetup.PointLightShadowMapsGenerationData.Size(); i++) {
			for (uint32 Face = 0; Face < 6; Face++) {
				auto& Cube = LightSetup.PointLightShadowMapDSVs[i];
				CmdList.ClearDepthStencilView(Cube[Face].Get(), CEDepthStencilF(1.0f, 0));
				CmdList.SetRenderTargets(nullptr, 0, Cube[Face].Get());

				auto& Data = LightSetup.PointLightShadowMapsGenerationData[i];
				PerShadowMapData.Matrix = Data.Matrix[Face];
				PerShadowMapData.Position = Data.Position;
				PerShadowMapData.FarPlane = Data.FarPlane;

				CmdList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
				                         CEResourceState::CopyDest);

				CmdList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &PerShadowMapData);

				CmdList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
				                         CEResourceState::VertexAndConstantBuffer);

				CmdList.SetConstantBuffer(PointLightVertexShader.Get(), PerShadowMapBuffer.Get(), 0);
				CmdList.SetConstantBuffer(PointLightPixelShader.Get(), PerShadowMapBuffer.Get(), 0);

				// Draw all objects to depthbuffer
				Core::Platform::Generic::Debug::CEConsoleVariable* GlobalFrustumCullEnabled = GTypedConsole.
					FindVariable("CE.FrustumCullEnabled");
				if (GlobalFrustumCullEnabled->GetBool()) {
					Render::Scene::CEFrustum CameraFrustum = Render::Scene::CEFrustum(
						Data.FarPlane, Data.ViewMatrix[Face], Data.ProjMatrix[Face]);
					for (const Main::Rendering::CEMeshDrawCommand& Command : Scene.GetMeshDrawCommands()) {
						const XMFLOAT4X4& Transform = Command.CurrentActor->GetTransform().GetMatrix().Native;
						XMMATRIX XmTransform = XMMatrixTranspose(XMLoadFloat4x4(&Transform));
						XMVECTOR XmTop = XMVectorSetW(XMLoadFloat3(&Command.Mesh->BoundingBox.Top.Native), 1.0f);
						XMVECTOR XmBottom = XMVectorSetW(XMLoadFloat3(&Command.Mesh->BoundingBox.Bottom.Native), 1.0f);
						XmTop = XMVector4Transform(XmTop, XmTransform);
						XmBottom = XMVector4Transform(XmBottom, XmTransform);

						Render::Scene::CEAABB Box;
						XMStoreFloat3(&Box.Top.Native, XmTop);
						XMStoreFloat3(&Box.Bottom.Native, XmBottom);
						if (CameraFrustum.CheckAABB(Box)) {
							CmdList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
							CmdList.SetIndexBuffer(Command.IndexBuffer);

							ShadowPerObjectBuffer.Matrix = Command.CurrentActor->GetTransform().GetMatrix().Native;
							ShadowPerObjectBuffer.ShadowOffset = Command.Mesh->ShadowOffset;

							CmdList.Set32BitShaderConstants(PointLightVertexShader.Get(), &ShadowPerObjectBuffer, 17);

							CmdList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
						}
					}
				}
				else {
					for (const Main::Rendering::CEMeshDrawCommand& Command : Scene.GetMeshDrawCommands()) {
						CmdList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
						CmdList.SetIndexBuffer(Command.IndexBuffer);

						ShadowPerObjectBuffer.Matrix = Command.CurrentActor->GetTransform().GetMatrix().Native;
						ShadowPerObjectBuffer.ShadowOffset = Command.Mesh->ShadowOffset;

						CmdList.Set32BitShaderConstants(PointLightVertexShader.Get(), &ShadowPerObjectBuffer, 17);

						CmdList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
					}
				}
			}
		}

		UpdatePointLight = false;
	}

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End Render PointLight ShadowMaps");

	CmdList.TransitionTexture(LightSetup.PointLightShadowMaps.Get(), CEResourceState::DepthWrite,
	                          CEResourceState::NonPixelShaderResource);
}

void CEDXBaseShadowMapRenderer::RenderDirectionalLightShadows(
	Main::RenderLayer::CECommandList& CmdList, const Main::Rendering::CEBaseLightSetup& LightSetup,
	const Render::Scene::CEScene& Scene) {
	//DirLightFrame++;
	//if (DirLightFrame > 6)
	//{
	//    UpdateDirLight = true;
	//    DirLightFrame = 0;
	//}

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin Update DirectionalLightBuffer");

	CmdList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin Render DirectionalLight ShadowMaps");

	//if (UpdateDirLight)
	//{
	TRACE_SCOPE("Render DirectionalLight ShadowMaps");

	CmdList.TransitionTexture(LightSetup.DirLightShadowMaps.Get(), CEResourceState::PixelShaderResource,
	                          CEResourceState::DepthWrite);

	CEDepthStencilView* DirLightDSV = LightSetup.DirLightShadowMaps->GetDepthStencilView();
	CmdList.ClearDepthStencilView(DirLightDSV, CEDepthStencilF(1.0f, 0));

	CmdList.SetRenderTargets(nullptr, 0, DirLightDSV);
	CmdList.SetGraphicsPipelineState(DirLightPipelineState.Get());

	CmdList.SetViewport(static_cast<float>(LightSetup.ShadowMapWidth), static_cast<float>(LightSetup.ShadowMapHeight),
	                    0.0f, 1.0f, 0.0f, 0.0f);
	CmdList.SetScissorRect(LightSetup.ShadowMapWidth, LightSetup.ShadowMapHeight, 0, 0);

	CmdList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);

	// PerObject Structs
	struct ShadowPerObject {
		XMFLOAT4X4 Matrix;
		float ShadowOffset;
	} ShadowPerObjectBuffer;

	PerShadowMap PerShadowMapData;
	for (uint32 i = 0; i < LightSetup.DirLightShadowMapsGenerationData.Size(); i++) {
		auto& Data = LightSetup.DirLightShadowMapsGenerationData[i];
		PerShadowMapData.Matrix = Data.Matrix;
		PerShadowMapData.Position = Data.Position;
		PerShadowMapData.FarPlane = Data.FarPlane;

		CmdList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
		                         CEResourceState::CopyDest);

		CmdList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &PerShadowMapData);

		CmdList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
		                         CEResourceState::VertexAndConstantBuffer);

		CmdList.SetConstantBuffers(DirLightShader.Get(), &PerShadowMapBuffer, 1, 0);

		// Draw all objects to depthbuffer
		for (const Main::Rendering::CEMeshDrawCommand& Command : Scene.GetMeshDrawCommands()) {
			CmdList.SetVertexBuffers(&Command.VertexBuffer, 1, 0);
			CmdList.SetIndexBuffer(Command.IndexBuffer);

			ShadowPerObjectBuffer.Matrix = Command.CurrentActor->GetTransform().GetMatrix().Native;
			ShadowPerObjectBuffer.ShadowOffset = Command.Mesh->ShadowOffset;

			CmdList.Set32BitShaderConstants(DirLightShader.Get(), &ShadowPerObjectBuffer, 17);

			CmdList.DrawIndexedInstanced(Command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
		}
	}

	//    UpdateDirLight = false;
	//}
	//else
	//{
	//    CmdList.BindPrimitiveTopology(EPrimitiveTopology::TriangleList);
	//}

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End Render DirectionalLight ShadowMaps");

	CmdList.TransitionTexture(LightSetup.DirLightShadowMaps.Get(), CEResourceState::DepthWrite,
	                          CEResourceState::NonPixelShaderResource);
}
