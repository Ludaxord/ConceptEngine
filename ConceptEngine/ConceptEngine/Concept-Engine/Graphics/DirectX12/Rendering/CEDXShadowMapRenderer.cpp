#include "CEDXShadowMapRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"

#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

#include "../../../Render/Scene/CEFrustum.h"

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
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	PerShadowMapBuffer->SetName("Per Shadow Map Buffer");

	Core::Containers::CEArray<uint8> shaderCode;
	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "VSMain", nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!PointLightVertexShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightVertexShader->SetName("Linear Shadow Map Vertex Shader");

		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "PSMain", nullptr,
		                                     RenderLayer::CEShaderStage::Pixel, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader = CastGraphicsManager()->CreatePixelShader(shaderCode);
		if (!PointLightPixelShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPixelShader->SetName("Linear Shadow Map Pixel Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		Core::Common::CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;

		Core::Common::CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
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
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		PointLightPipelineState->SetName("Linear Shadow Map Pipeline State");
	}

	{
		if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ShadowMap.hlsl", "Main", nullptr,
		                                     RenderLayer::CEShaderStage::Vertex, RenderLayer::CEShaderModel::SM_6_0,
		                                     shaderCode)) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		DirLightShader = CastGraphicsManager()->CreateVertexShader(shaderCode);
		if (!DirLightShader) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		DirLightShader->SetName("Shadow Map Vertex Shader");

		RenderLayer::CEDepthStencilStateCreateInfo depthStencilStateInfo;
		depthStencilStateInfo.DepthFunc = RenderLayer::CEComparisonFunc::LessEqual;
		depthStencilStateInfo.DepthEnable = true;
		depthStencilStateInfo.DepthWriteMask = RenderLayer::CEDepthWriteMask::All;

		Core::Common::CERef<RenderLayer::CEDepthStencilState> depthStencilState = CastGraphicsManager()->
			CreateDepthStencilState(depthStencilStateInfo);
		if (!depthStencilState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		depthStencilState->SetName("Shadow Depth Stencil State");

		RenderLayer::CERasterizerStateCreateInfo rasterizerStateInfo;
		rasterizerStateInfo.CullMode = RenderLayer::CECullMode::Back;

		Core::Common::CERef<RenderLayer::CERasterizerState> rasterizerState = CastGraphicsManager()->
			CreateRasterizerState(rasterizerStateInfo);
		if (!rasterizerState) {
			Core::Debug::CEDebug::DebugBreak();
			return false;
		}

		rasterizerState->SetName("Shadow Rasterizer State");

		RenderLayer::CEBlendStateCreateInfo blendStateInfo;
		Core::Common::CERef<RenderLayer::CEBlendState> blendState = CastGraphicsManager()->CreateBlendState(
			blendStateInfo);
		if (!blendState) {
			Core::Debug::CEDebug::DebugBreak();
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
			Core::Debug::CEDebug::DebugBreak();
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
				auto& cube = lightSetup.PointLightShadowMapDSVs[i];
				commandList.ClearDepthStencilView(cube[face].Get(), CEDepthStencilF(1.0f, 0));;
				commandList.SetRenderTargets(nullptr, 0, cube[face].Get());

				auto& data = lightSetup.PointLightShadowMapsGenerationData[i];
				perShadowMapData.Matrix = data.Matrix[face].Native;
				perShadowMapData.Position = data.Position.Native;
				perShadowMapData.FarPlane = data.FarPlane;

				commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
				                             CEResourceState::CopyDest);
				commandList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &perShadowMapData);
				commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
				                             CEResourceState::VertexAndConstantBuffer);

				commandList.SetConstantBuffer(PointLightVertexShader.Get(), PerShadowMapBuffer.Get(), 0);
				commandList.SetConstantBuffer(PointLightPixelShader.Get(), PerShadowMapBuffer.Get(), 0);

				Core::Platform::Generic::Debug::CEConsoleVariable* globalFrustumCullEnabled = GTypedConsole.
					FindVariable("CE.EnableFrustumCulling");
				if (globalFrustumCullEnabled->GetBool()) {
					Render::Scene::CEFrustum cameraFrustum = Render::Scene::CEFrustum(data.FarPlane,
						data.ViewMatrix[face],
						data.ProjMatrix[face]
					);

					for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
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
					}
				}
				else {
					for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
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
		DirectX::XMFLOAT4X4 Matrix;
		float ShadowOffset;
	} ShadowPerObjectBuffer;

	PerShadowMap perShadowMapData;
	for (uint32 i = 0; i < lightSetup.DirLightShadowMapGenerationData.Size(); i++) {
		auto& data = lightSetup.DirLightShadowMapGenerationData[i];
		perShadowMapData.Matrix = data.Matrix.Native;
		perShadowMapData.Position = data.Position.Native;
		perShadowMapData.FarPlane = data.FarPlane;

		commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
		                             CEResourceState::CopyDest);
		commandList.UpdateBuffer(PerShadowMapBuffer.Get(), 0, sizeof(PerShadowMap), &perShadowMapData);
		commandList.TransitionBuffer(PerShadowMapBuffer.Get(), CEResourceState::CopyDest,
		                             CEResourceState::VertexAndConstantBuffer);

		commandList.SetConstantBuffers(DirLightShader.Get(), &PerShadowMapBuffer, 1, 0);

		for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
			commandList.SetVertexBuffers(&command.VertexBuffer, 1, 0);
			commandList.SetIndexBuffer(command.IndexBuffer);

			ShadowPerObjectBuffer.Matrix = command.CurrentActor->GetTransform().GetMatrix().Native;
			ShadowPerObjectBuffer.ShadowOffset = command.Mesh->ShadowOffset;

			commandList.Set32BitShaderConstants(DirLightShader.Get(), &ShadowPerObjectBuffer, 17);

			commandList.DrawIndexedInstanced(command.IndexBuffer->GetNumIndices(), 1, 0, 0, 0);
		}
	}

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== END RENDER DIRECTIONAL LIGHT SHADOW MAPS ==");
	commandList.TransitionTexture(lightSetup.DirLightShadowMaps.Get(), CEResourceState::DepthWrite,
	                              CEResourceState::NonPixelShaderResource);

}

bool CEDXShadowMapRenderer::CreateShadowMaps(Main::Rendering::CELightSetup& frameResources) {
	frameResources.PointLightShadowMaps = CastGraphicsManager()->CreateTextureCubeArray(
		frameResources.ShadowMapFormat,
		frameResources.PointLightShadowSize,
		1,
		frameResources.MaxPointLightShadows,
		TextureFlags_ShadowMap, CEResourceState::PixelShaderResource, nullptr);

	if (!frameResources.PointLightShadowMaps) {
		return false;
	}

	frameResources.PointLightShadowMaps->SetName("Point Light Shadow Maps");

	frameResources.PointLightShadowMapDSVs.Resize(frameResources.MaxPointLightShadows);

	for (uint32 i = 0; i < frameResources.MaxPointLightShadows; i++) {
		for (uint32 face = 0; face < 6; face++) {
			Core::Containers::CEStaticArray<Core::Common::CERef<CEDepthStencilView>, 6>& depthCube = frameResources.
				PointLightShadowMapDSVs[i];
			depthCube[face] = CastGraphicsManager()->CreateDepthStencilViewForTextureCubeArray(
				frameResources.PointLightShadowMaps.Get(),
				frameResources.ShadowMapFormat,
				i,
				0,
				GetCubeFaceFromIndex(face)
			);
			if (!depthCube[face]) {
				Core::Debug::CEDebug::DebugBreak();
				return false;
			}
		}
	}

	frameResources.DirLightShadowMaps = CastGraphicsManager()->CreateTexture2D(
		frameResources.ShadowMapFormat,
		frameResources.ShadowMapWidth,
		frameResources.ShadowMapHeight,
		1,
		1,
		TextureFlags_ShadowMap,
		CEResourceState::PixelShaderResource,
		nullptr
	);
	if (!frameResources.DirLightShadowMaps) {
		return false;
	}

	frameResources.DirLightShadowMaps->SetName("Directional Light Shadow Maps");

	return true;
}
