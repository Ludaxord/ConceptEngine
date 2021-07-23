#include "CEDXShadowMapRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"

#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../Render/Scene/CEFrustum.h"
#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

struct PerShadowMap {
	DirectX::XMFLOAT4X4 Matrix;
	DirectX::XMFLOAT3 Position;
	float FarPlane;
};

bool CEDXShadowMapRenderer::Create(Main::Rendering::CELightSetup& lightSetup,
                                   Main::Rendering::CEFrameResources& resources) {
	if (!CreateShadowMaps(lightSetup)) {
		return false;
	}

	PerShadowMapBuffer = CastGraphicsManager()->CreateConstantBuffer<PerShadowMap>(
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);
	if (!PerShadowMapBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	PerShadowMapBuffer->SetName("Per Shadow Map Buffer");

	CEArray<uint8> shaderCode;
	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "VSMain",
		                                     nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!PointLightVertexShader) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader->SetName("Linear Shadow Map Vertex Shader");

		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "PSMain",
		                                     nullptr,
		                                     RenderLayer::CEShaderStage::Pixel, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
		if (!PointLightPixelShader) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader->SetName("Linear Shadow Map Pixel Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;

		CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Shadow Blend State");

		RenderLayer::CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
		pipelineStateInfo.BlendState = blendState.Get();
		pipelineStateInfo.DepthStencilState = depthStencilState.Get();
		pipelineStateInfo.IBStripCutValue = RenderLayer::CEIndexBufferStripCutValue::Disabled;
		pipelineStateInfo.InputLayoutState = resources.StdInputLayout.Get();
		pipelineStateInfo.PrimitiveTopologyType = RenderLayer::CEPrimitiveTopologyType::Triangle;
		pipelineStateInfo.RasterizerState = rasterizerState.Get();
		pipelineStateInfo.SampleCount = 1;
		pipelineStateInfo.SampleQuality = 0;
		pipelineStateInfo.SampleMask = 0xffffffff;
		pipelineStateInfo.ShaderState.VertexShader = PointLightVertexShader.Get();
		pipelineStateInfo.ShaderState.PixelShader = PointLightPixelShader.Get();
		pipelineStateInfo.PipelineFormats.NumRenderTargets = 0;
		pipelineStateInfo.PipelineFormats.DepthStencilFormat = lightSetup.ShadowMapFormat;

		PointLightPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(pipelineStateInfo);
		if (!PointLightPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}

		PointLightPipelineState->SetName("Linear Shadow Map Pipeline State");
	}

	{
		if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ShadowMap.hlsl"), "Main",
		                                     nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			CEDebug::DebugBreak();
			return false;
		}

		DirLightShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!DirLightShader) {
			CEDebug::DebugBreak();
			return false;
		}

		DirLightShader->SetName("Shadow Map Vertex Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			CEDebug::DebugBreak();
			return false;
		}

		blendState->SetName("Shadow Blend State");

		RenderLayer::CEGraphicsPipelineStateCreateInfo pipelineStateInfo;
		pipelineStateInfo.BlendState = blendState.Get();
		pipelineStateInfo.DepthStencilState = depthStencilState.Get();
		pipelineStateInfo.IBStripCutValue = RenderLayer::CEIndexBufferStripCutValue::Disabled;
		pipelineStateInfo.InputLayoutState = resources.StdInputLayout.Get();
		pipelineStateInfo.PrimitiveTopologyType = RenderLayer::CEPrimitiveTopologyType::Triangle;
		pipelineStateInfo.RasterizerState = rasterizerState.Get();
		pipelineStateInfo.SampleCount = 1;
		pipelineStateInfo.SampleQuality = 0;
		pipelineStateInfo.SampleMask = 0xffffffff;
		pipelineStateInfo.ShaderState.VertexShader = DirLightShader.Get();
		pipelineStateInfo.ShaderState.PixelShader = nullptr;
		pipelineStateInfo.PipelineFormats.NumRenderTargets = 0;
		pipelineStateInfo.PipelineFormats.DepthStencilFormat = lightSetup.ShadowMapFormat;

		DirLightPipelineState = CastGraphicsManager()->CreateGraphicsPipelineState(pipelineStateInfo);
		if (!DirLightPipelineState) {
			CEDebug::DebugBreak();
			return false;
		}

		DirLightPipelineState->SetName("Shadow Map Pipeline State");
	}

	return true;
}

void CEDXShadowMapRenderer::RenderPointLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                    const Main::Rendering::CELightSetup& lightSetup,
                                                    const Render::Scene::CEScene& scene) {
	PointLightFrame++;
	if (PointLightFrame > 6) {
		UpdatePointLight = true;
		PointLightFrame = 0;
	}

	commandList.SetPrimitiveTopology(RenderLayer::CEPrimitiveTopology::TriangleList);
	commandList.TransitionTexture(lightSetup.PointLightShadowMaps.Get(),
	                              RenderLayer::CEResourceState::PixelShaderResource,
	                              RenderLayer::CEResourceState::DepthWrite);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN RENDER POINT LIGHT SHADOW MAP ==");

	if (UpdatePointLight) {
		TRACE_SCOPE("== RENDER POINT LIGHT SHADOW MAPS");

		const uint32 pointLightShadowSize = lightSetup.PointLightShadowSize;
		commandList.SetViewport(static_cast<float>(pointLightShadowSize), static_cast<float>(pointLightShadowSize),
		                        0.0f, 1.0f, 0.0f, 0.0f);
		commandList.SetScissorRect(static_cast<float>(pointLightShadowSize), static_cast<float>(pointLightShadowSize),
		                           0, 0);

		commandList.SetGraphicsPipelineState(PointLightPipelineState.Get());

		struct ShadowPerObject {
			DirectX::XMFLOAT4X4 Matrix;
			float ShadowOffset;
		} ShadowPerObjectBuffer;

		PerShadowMap perShadowMapData;
		for (uint32 i = 0; i < lightSetup.PointLightShadowMapsGenerationData.Size(); i++) {
			for (uint32 face = 0; face < 6; face++) {
				commandList.SetDebugPoint(
					"Renderer ShadowMapRenderer PointLightShadowMapsGenerationData Face: " + std::to_string(face) +
					"' and index " +
					std::to_string(i));

				auto& cube = lightSetup.PointLightShadowMapDSVs[i];

				if (cube[face] == nullptr || !cube[face]) {

					commandList.SetDebugPoint(
						"Renderer ShadowMapRenderer Cube Face at: '" + std::to_string(face) + "' and index " +
						std::to_string(i) + " is Empty");
				}

				commandList.ClearDepthStencilView(cube[face].Get(), CEDepthStencilF(1.0f, 0));;
				commandList.SetRenderTargets(nullptr, 0, cube[face].Get());

				auto& data = lightSetup.PointLightShadowMapsGenerationData[i];
				perShadowMapData.Matrix = data.Matrix[face];
				perShadowMapData.Position = data.Position;
				perShadowMapData.FarPlane = data.FarPlane;

				commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
				                             CEResourceState::CopyDest);
				commandList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &perShadowMapData);
				commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
				                             CEResourceState::VertexAndConstantBuffer);

				commandList.SetConstantBuffer(PointLightVertexShader.Get(), PerShadowMapBuffer.Get(), 0);
				commandList.SetConstantBuffer(PointLightPixelShader.Get(), PerShadowMapBuffer.Get(), 0);

				Core::Platform::Generic::Debug::CEConsoleVariable* globalFrustumCullEnabled = GTypedConsole.
					FindVariable("CE.FrustumCullEnabled");
				if (globalFrustumCullEnabled->GetBool()) {
					Render::Scene::CEFrustum cameraFrustum = Render::Scene::CEFrustum(data.FarPlane,
						CEMatrixFloat4X4(data.ViewMatrix[face]),
						CEMatrixFloat4X4(data.ProjMatrix[face])
					);

					int indx = 0;
					for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
						// commandList.SetDebugPoint(
						// 	"Renderer globalFrustumCullEnabled ShadowMapRenderer GetMeshDrawCommands Index: " +
						// 	std::to_string(indx));
						CEMatrixFloat4X4 transform = command.CurrentActor->GetTransform().GetMatrix();
						auto ceTransform = CEMatrixTranspose(CELoadFloat4X4(&transform));
						auto ceTop = CEVectorSetW(CELoadFloat3(&command.Mesh->BoundingBox.Top), 1.0f);
						auto ceBottom = CEVectorSetW(CELoadFloat3(&command.Mesh->BoundingBox.Bottom), 1.0f);
						ceTop = CEVector4Transform(ceTop, ceTransform);
						ceBottom = CEVector4Transform(ceBottom, ceTransform);

						Render::Scene::CEAABB box;
						CEStoreFloat3(&box.Top, ceTop);
						CEStoreFloat3(&box.Bottom, ceBottom);
						if (cameraFrustum.CheckAABB(box)) {
							commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
							commandList.SetIndexBuffer(command.IndexBuffer);

							ShadowPerObjectBuffer.Matrix = command.CurrentActor->GetTransform().GetMatrix().Native;
							ShadowPerObjectBuffer.ShadowOffset = command.Mesh->ShadowOffset;

							commandList.Set32BitShaderConstants(PointLightVertexShader.Get(), &ShadowPerObjectBuffer,
							                                    17);
							commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
						}
						indx++;
					}
				}
				else {
					for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {

						commandList.SetDebugPoint(
							"Renderer globalFrustumCullDISABLED ShadowMapRenderer GetMeshDrawCommands Index: ");
						commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
						commandList.SetIndexBuffer(command.IndexBuffer);

						ShadowPerObjectBuffer.Matrix = command.CurrentActor->GetTransform().GetMatrix().Native;
						ShadowPerObjectBuffer.ShadowOffset = command.Mesh->ShadowOffset;

						commandList.Set32BitShaderConstants(PointLightVertexShader.Get(), &ShadowPerObjectBuffer, 17);
						commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
					}
				}
			}
		}

		UpdatePointLight = false;
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END RENDER POINT LIGHT SHADOW MAPS ==");

	commandList.TransitionTexture(lightSetup.PointLightShadowMaps.Get(), CEResourceState::DepthWrite,
	                              CEResourceState::NonPixelShaderResource);
}

void CEDXShadowMapRenderer::RenderDirectionalLightShadows(Main::RenderLayer::CECommandList& commandList,
                                                          const Main::Rendering::CELightSetup& lightSetup,
                                                          const Render::Scene::CEScene& scene) {
	commandList.SetDebugPoint(":::: Renderer ShadowMapRenderer RenderDirectionalLightShadows BEGIN ::::");
	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN UPDATE DIRECTIONAL LIGHT BUFFER ==");
	commandList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN RENDER DIRECTIONAL LIGHT SHADOW MAPS ==");

	TRACE_SCOPE("== RENDER DIRECTIONAL LIGHT SHADOW MAPS ==");

	commandList.TransitionTexture(lightSetup.DirLightShadowMaps.Get(), CEResourceState::PixelShaderResource,
	                              CEResourceState::DepthWrite);

	CEDepthStencilView* dirLightDSV = lightSetup.DirLightShadowMaps->GetDepthStencilView();

	commandList.ClearDepthStencilView(dirLightDSV, CEDepthStencilF(1.0f, 0));

	commandList.SetRenderTargets(nullptr, 0, dirLightDSV);
	commandList.SetGraphicsPipelineState(DirLightPipelineState.Get());

	commandList.SetViewport(static_cast<float>(lightSetup.ShadowMapWidth),
	                        static_cast<float>(lightSetup.ShadowMapHeight), 0.0f, 1.0f, 0.0f, 0.0f);
	commandList.SetScissorRect(lightSetup.ShadowMapWidth, lightSetup.ShadowMapHeight, 0, 0);

	commandList.SetPrimitiveTopology(CEPrimitiveTopology::TriangleList);

	struct ShadowPerObject {
		XMFLOAT4X4 Matrix;
		float ShadowOffset;
	} ShadowPerObjectBuffer;

	PerShadowMap perShadowMapData;
	for (uint32 i = 0; i < lightSetup.DirLightShadowMapGenerationData.Size(); i++) {
		auto& data = lightSetup.DirLightShadowMapGenerationData[i];
		perShadowMapData.Matrix = data.Matrix;
		perShadowMapData.Position = data.Position;
		perShadowMapData.FarPlane = data.FarPlane;

		commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
		                             CEResourceState::CopyDest);
		commandList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &perShadowMapData);
		commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
		                             CEResourceState::VertexAndConstantBuffer);

		commandList.SetConstantBuffers(DirLightShader.Get(), &PerShadowMapBuffer, 1, 0);

		int indx = 0;
		for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
			commandList.SetDebugPoint(
				"Renderer ShadowMapRenderer RenderDirectionalLightShadows GetMeshDrawCommands Index: " +
				std::to_string(indx));

			//TODO: FIX [ID3D12CommandList::DrawIndexedInstanced: The depth stencil format does not match that specified by the current pipeline state.]
			commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
			commandList.SetIndexBuffer(command.IndexBuffer);

			ShadowPerObjectBuffer.Matrix = command.CurrentActor->GetTransform().GetMatrix().Native;
			ShadowPerObjectBuffer.ShadowOffset = command.Mesh->ShadowOffset;

			commandList.Set32BitShaderConstants(DirLightShader.Get(), &ShadowPerObjectBuffer, 17);

			commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
			indx++;
		}
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END RENDER DIRECTIONAL LIGHT SHADOW MAPS ==");
	commandList.TransitionTexture(lightSetup.DirLightShadowMaps.Get(), CEResourceState::DepthWrite,
	                              CEResourceState::NonPixelShaderResource);

}

bool CEDXShadowMapRenderer::CreateShadowMaps(Main::Rendering::CELightSetup& LightSetup) {
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
				CEStaticArray<CERef<CEDepthStencilView>, 6>& DepthCube = LightSetup.PointLightShadowMapDSVs[i];

				std::string Name = "ShadowMapDepthStencilCubeMap Face " + std::to_string(Face) + " Index " +
					std::to_string(i);
				DepthCube[Face] = CastGraphicsManager()->CreateDepthStencilViewForTextureCubeArray(
					LightSetup.PointLightShadowMaps.Get(),
					LightSetup.ShadowMapFormat, 0, i, GetCubeFaceFromIndex(Face), Name);
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
