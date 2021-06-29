#include "CELightSetup.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Render/Scene/Lights/CEPointLight.h"
#include "../../../Render/Scene/Lights/CEDirectionalLight.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

using namespace ConceptEngine::Graphics::Main::Rendering;

bool CELightSetup::Create() {
	DirectionalLightsData.Reserve(1);
	DirectionalLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
		DirectionalLightsData.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);

	if (!DirectionalLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	DirectionalLightsBuffer->SetName("Directional Light Buffer");

	PointLightsData.Reserve(256);
	PointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
		PointLightsData.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);

	if (!PointLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	PointLightsBuffer->SetName("Point Light Buffer");

	PointLightsPosRad.Reserve(256);
	PointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
		PointLightsPosRad.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);

	if (!PointLightsPosRadBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	
	PointLightsPosRadBuffer->SetName("Point Lights Pos Rad Buffer");

	ShadowCastingPointLightsData.Reserve(8);
	ShadowCastingPointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
		ShadowCastingPointLightsData.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);

	if (!ShadowCastingPointLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadowCastingPointLightsBuffer->SetName("Shadow Casting Point Lights Buffer");

	ShadowCastingPointLightsPosRad.Reserve(8);
	ShadowCastingPointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
		ShadowCastingPointLightsData.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		RenderLayer::CEResourceState::VertexAndConstantBuffer,
		nullptr
	);

	if (!ShadowCastingPointLightsPosRadBuffer) {
		CEDebug::DebugBreak();
		return false;
	}

	ShadowCastingPointLightsPosRadBuffer->SetName("Shadow Casting Point Lights Pos Rad Buffer");

	return true;
}

void CELightSetup::BeginFrame(RenderLayer::CECommandList& commandList, const Render::Scene::CEScene& scene) {
	PointLightsPosRad.Clear();
	PointLightsData.Clear();
	ShadowCastingPointLightsPosRad.Clear();
	ShadowCastingPointLightsData.Clear();
	PointLightShadowMapsGenerationData.Clear();
	DirLightShadowMapGenerationData.Clear();
	DirectionalLightsData.Clear();

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== BEGIN UPDATE LIGHTS ==");

	TRACE_SCOPE("Update Light Buffers");

	for (Render::Scene::Lights::CELight* light : scene.GetLights()) {
		float intensity = light->GetIntensity();
		CEVectorFloat3 color = light->GetColor();
		color = color * intensity;
		if (IsSubClassOf<Render::Scene::Lights::CEPointLight>(light)) {
			Render::Scene::Lights::CEPointLight* currentLight = Cast<Render::Scene::Lights::CEPointLight>(light);
			Assert(currentLight != nullptr);

			//TODO: add it to creator and project properties
			constexpr float minLuma = 0.005f;
			float dot = color.x * 0.2126f + color.y * 0.7152f + color.z * 0.0722f;
			float radius = sqrt(dot / minLuma);

			CEVectorFloat3 position = currentLight->GetPosition();
			CEVectorFloat4 positionRad = CEVectorFloat4(position.x, position.y, position.z, radius);

			if (currentLight->IsShadowCaster()) {
				CEShadowCastingPointLightData data;
				data.Color = color;
				data.FarPlane = currentLight->GetShadowFarPlane();
				data.MaxShadowBias = currentLight->GetMaxShadowBias();
				data.ShadowBias = currentLight->GetShadowBias();

				ShadowCastingPointLightsData.EmplaceBack(data);
				ShadowCastingPointLightsPosRad.EmplaceBack(positionRad);

				CEPointLightShadowMapGenerationData shadowMapData;
				shadowMapData.FarPlane = currentLight->GetShadowFarPlane();
				shadowMapData.Position = currentLight->GetPosition();

				for (uint32 face = 0; face < 6; face++) {
					shadowMapData.Matrix[face] = currentLight->GetMatrix(face);
					shadowMapData.ViewMatrix[face] = currentLight->GetViewMatrix(face);
					shadowMapData.ProjMatrix[face] = currentLight->GetProjectionMatrix(face);
				}

				PointLightShadowMapsGenerationData.EmplaceBack(shadowMapData);
			}
			else {
				CEPointLightData data;
				data.Color = color;

				PointLightsData.EmplaceBack(data);
				PointLightsPosRad.EmplaceBack(positionRad);
			}
		}
		else if (IsSubClassOf<Render::Scene::Lights::CEDirectionalLight>(light)) {
			Render::Scene::Lights::CEDirectionalLight* currentLight = Cast<Render::Scene::Lights::CEDirectionalLight>(light);
			Assert(currentLight != nullptr);

			CEDirectionalLightData data;
			data.Color = color;
			data.ShadowBias = currentLight->GetShadowBias();
			data.Direction = currentLight->GetDirection();

			CEVectorFloat3 cameraPosition = scene.GetCamera()->GetPosition();
			CEVectorFloat3 cameraForward = scene.GetCamera()->GetForward();

			float nearPlane = scene.GetCamera()->GetNearPlane();

			//TODO: add it to creator and project properties
			float dirFrustum = 35.0f;
			CEVectorFloat3 lookAt = cameraPosition + (cameraForward * (dirFrustum));
			currentLight->SetLookAt(lookAt);

			data.LightMatrix = currentLight->GetMatrix();
			data.MaxShadowBias = currentLight->GetMaxShadowBias();

			DirectionalLightsData.EmplaceBack(data);

			CEDirLightShadowMapGenerationData shadowMapData;
			shadowMapData.Matrix = currentLight->GetMatrix();
			shadowMapData.FarPlane = currentLight->GetShadowFarPlane();
			shadowMapData.Position = currentLight->GetShadowMapPosition();

			DirLightShadowMapGenerationData.EmplaceBack(shadowMapData);
		}
	}

	if (DirectionalLightsData.SizeInBytes() > DirectionalLightsBuffer->GetSize()) {
		commandList.DiscardResource(DirectionalLightsBuffer.Get());

		DirectionalLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
			DirectionalLightsData.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr
		);
		if (!DirectionalLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (PointLightsData.SizeInBytes() > PointLightsBuffer->GetSize()) {
		commandList.DiscardResource(PointLightsBuffer.Get());

		PointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
			PointLightsData.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr
		);
		if (!PointLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (PointLightsPosRad.SizeInBytes() > PointLightsPosRadBuffer->GetSize()) {
		commandList.DiscardResource(PointLightsPosRadBuffer.Get());

		PointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
			PointLightsPosRad.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr
		);
		if (!PointLightsPosRadBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (ShadowCastingPointLightsData.SizeInBytes() > ShadowCastingPointLightsBuffer->GetSize()) {
		commandList.DiscardResource(ShadowCastingPointLightsBuffer.Get());

		ShadowCastingPointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
			ShadowCastingPointLightsData.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr
		);

		if (!ShadowCastingPointLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (ShadowCastingPointLightsPosRad.SizeInBytes() > ShadowCastingPointLightsPosRadBuffer->GetSize()) {
		commandList.DiscardResource(ShadowCastingPointLightsPosRadBuffer.Get());

		ShadowCastingPointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
			ShadowCastingPointLightsPosRad.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr
		);

		if (!ShadowCastingPointLightsPosRadBuffer) {
			CEDebug::DebugBreak();
		}
	}

	commandList.TransitionBuffer(DirectionalLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);
	commandList.TransitionBuffer(PointLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);
	commandList.TransitionBuffer(PointLightsPosRadBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);
	commandList.TransitionBuffer(ShadowCastingPointLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);
	commandList.TransitionBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                             CEResourceState::CopyDest);

	if (!DirectionalLightsData.IsEmpty()) {
		commandList.UpdateBuffer(DirectionalLightsBuffer.Get(), 0, DirectionalLightsData.SizeInBytes(),
		                         DirectionalLightsData.Data());
	}

	if (!PointLightsData.IsEmpty()) {
		commandList.UpdateBuffer(PointLightsBuffer.Get(), 0, PointLightsData.SizeInBytes(), PointLightsData.Data());
		commandList.UpdateBuffer(PointLightsPosRadBuffer.Get(), 0, PointLightsPosRad.SizeInBytes(),
		                         PointLightsPosRad.Data());
	}

	if (!ShadowCastingPointLightsData.IsEmpty()) {
		commandList.UpdateBuffer(ShadowCastingPointLightsBuffer.Get(), 0, ShadowCastingPointLightsData.SizeInBytes(),
		                         ShadowCastingPointLightsData.Data());
		commandList.UpdateBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), 0,
		                         ShadowCastingPointLightsPosRad.SizeInBytes(), ShadowCastingPointLightsPosRad.Data());
	}

	commandList.TransitionBuffer(DirectionalLightsBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);
	commandList.TransitionBuffer(PointLightsBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);
	commandList.TransitionBuffer(PointLightsPosRadBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);
	commandList.TransitionBuffer(ShadowCastingPointLightsBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);
	commandList.TransitionBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), CEResourceState::CopyDest,
	                             CEResourceState::VertexAndConstantBuffer);

	INSERT_DEBUG_CMDLIST_MARKER(commandList, "== ENMD UPDATE LIGHTS ==");
}

void CELightSetup::Release() {
	PointLightsPosRadBuffer.Reset();
	PointLightsBuffer.Reset();

	ShadowCastingPointLightsPosRadBuffer.Reset();
	ShadowCastingPointLightsBuffer.Reset();

	DirectionalLightsBuffer.Reset();

	PointLightShadowMaps.Reset();

	for (auto& dsvCube : PointLightShadowMapDSVs) {
		for (uint32 i = 0; i < 6; i++) {
			dsvCube[i].Reset();
		}
	}

	DirLightShadowMaps.Reset();

	IrradianceMap.Reset();
	IrradianceMapUAV.Reset();

	SpecularIrradianceMap.Reset();

	for (auto& UAV : SpecularIrradianceMapUAVs) {
		UAV.Reset();
	}
}
