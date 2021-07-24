#include "CEBaseLightSetup.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../../Core/Debug/CEProfiler.h"

#include "../../../Render/Scene/Lights/CEPointLight.h"
#include "../../../Render/Scene/Lights/CEDirectionalLight.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

bool ConceptEngine::Graphics::Main::Rendering::CEBaseLightSetup::Create() {
	DirectionalLightsData.Reserve(1);
	DirectionalLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
		DirectionalLightsData.CapacityInBytes(), BufferFlag_Default, CEResourceState::VertexAndConstantBuffer, nullptr);
	if (!DirectionalLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		DirectionalLightsBuffer->SetName("DirectionalLightsBuffer");
	}

	PointLightsData.Reserve(256);
	PointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(PointLightsData.CapacityInBytes(),
	                                                                BufferFlag_Default,
	                                                                CEResourceState::VertexAndConstantBuffer, nullptr);
	if (!PointLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PointLightsBuffer->SetName("PointLightsBuffer");
	}

	PointLightsPosRad.Reserve(256);
	PointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
		PointLightsPosRad.CapacityInBytes(), RenderLayer::BufferFlag_Default, CEResourceState::VertexAndConstantBuffer,
		nullptr);
	if (!PointLightsPosRadBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		PointLightsPosRadBuffer->SetName("PointLightsPosRadBuffer");
	}

	ShadowCastingPointLightsData.Reserve(8);
	ShadowCastingPointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
		ShadowCastingPointLightsData.CapacityInBytes(),
		BufferFlag_Default,
		CEResourceState::VertexAndConstantBuffer,
		nullptr);
	if (!ShadowCastingPointLightsBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		ShadowCastingPointLightsBuffer->SetName("ShadowCastingPointLightsBuffer");
	}

	ShadowCastingPointLightsPosRad.Reserve(8);
	ShadowCastingPointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
		ShadowCastingPointLightsPosRad.CapacityInBytes(),
		RenderLayer::BufferFlag_Default,
		CEResourceState::VertexAndConstantBuffer,
		nullptr);
	if (!ShadowCastingPointLightsPosRadBuffer) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		ShadowCastingPointLightsPosRadBuffer->SetName("ShadowCastingPointLightsPosRadBuffer");
	}

	return true;
}

void ConceptEngine::Graphics::Main::Rendering::CEBaseLightSetup::BeginFrame(RenderLayer::CECommandList& CmdList,
                                                                            const Render::Scene::CEScene& Scene) {
	PointLightsPosRad.Clear();
	PointLightsData.Clear();
	ShadowCastingPointLightsPosRad.Clear();
	ShadowCastingPointLightsData.Clear();
	PointLightShadowMapsGenerationData.Clear();
	DirLightShadowMapsGenerationData.Clear();
	DirectionalLightsData.Clear();

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "Begin Update Lights");

	TRACE_SCOPE("Update LightBuffers");

	for (Render::Scene::Lights::CELight* Light : Scene.GetLights()) {
		float Intensity = Light->GetIntensity();
		XMFLOAT3 Color = Light->GetColor().Native;
		Color = Color * Intensity;
		if (IsSubClassOf<Render::Scene::Lights::CEPointLight>(Light)) {
			Render::Scene::Lights::CEPointLight* CurrentLight = Cast<Render::Scene::Lights::CEPointLight>(Light);
			Assert(CurrentLight != nullptr);

			constexpr float MinLuma = 0.005f;
			float Dot = Color.x * 0.2126f + Color.y * 0.7152f + Color.z * 0.0722f;
			float Radius = sqrt(Dot / MinLuma);

			XMFLOAT3 Position = CurrentLight->GetPosition().Native;
			XMFLOAT4 PosRad = XMFLOAT4(Position.x, Position.y, Position.z, Radius);
			if (CurrentLight->IsShadowCaster()) {
				ShadowCastingPointLightData Data;
				Data.Color = Color;
				Data.FarPlane = CurrentLight->GetShadowFarPlane();
				Data.MaxShadowBias = CurrentLight->GetMaxShadowBias();
				Data.ShadowBias = CurrentLight->GetShadowBias();

				ShadowCastingPointLightsData.EmplaceBack(Data);
				ShadowCastingPointLightsPosRad.EmplaceBack(PosRad);

				PointLightShadowMapGenerationData ShadowMapData;
				ShadowMapData.FarPlane = CurrentLight->GetShadowFarPlane();
				ShadowMapData.Position = CurrentLight->GetPosition().Native;

				for (uint32 Face = 0; Face < 6; Face++) {
					ShadowMapData.Matrix[Face] = CurrentLight->GetMatrix(Face).Native;
					ShadowMapData.ViewMatrix[Face] = CurrentLight->GetViewMatrix(Face).Native;
					ShadowMapData.ProjMatrix[Face] = CurrentLight->GetProjectionMatrix(Face).Native;
				}

				PointLightShadowMapsGenerationData.EmplaceBack(ShadowMapData);
			}
			else {
				PointLightData Data;
				Data.Color = Color;

				PointLightsData.EmplaceBack(Data);
				PointLightsPosRad.EmplaceBack(PosRad);
			}
		}
		else if (IsSubClassOf<Render::Scene::Lights::CEDirectionalLight>(Light)) {
			Render::Scene::Lights::CEDirectionalLight* CurrentLight = Cast<
				Render::Scene::Lights::CEDirectionalLight>(Light);
			Assert(CurrentLight != nullptr);

			DirectionalLightData Data;
			Data.Color = Color;
			Data.ShadowBias = CurrentLight->GetShadowBias();
			Data.Direction = CurrentLight->GetDirection().Native;

			// TODO: Should not be the done in the renderer
			XMFLOAT3 CameraPosition = Scene.GetCamera()->GetPosition().Native;
			XMFLOAT3 CameraForward = Scene.GetCamera()->GetForward().Native;

			float Near = Scene.GetCamera()->GetNearPlane();
			float DirFrustum = 35.0f;
			XMFLOAT3 LookAt = CameraPosition + (CameraForward * (DirFrustum + Near));
			CurrentLight->SetLookAt(LookAt);

			Data.LightMatrix = CurrentLight->GetMatrix().Native;
			Data.MaxShadowBias = CurrentLight->GetMaxShadowBias();

			DirectionalLightsData.EmplaceBack(Data);

			DirLightShadowMapGenerationData ShadowData;
			ShadowData.Matrix = CurrentLight->GetMatrix().Native;
			ShadowData.FarPlane = CurrentLight->GetShadowFarPlane();
			ShadowData.Position = CurrentLight->GetShadowMapPosition().Native;

			DirLightShadowMapsGenerationData.EmplaceBack(ShadowData);
		}
	}

	if (DirectionalLightsData.SizeInBytes() > DirectionalLightsBuffer->GetSize()) {
		CmdList.DiscardResource(DirectionalLightsBuffer.Get());

		DirectionalLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
			DirectionalLightsData.CapacityInBytes(), BufferFlag_Default, CEResourceState::VertexAndConstantBuffer,
			nullptr);
		if (!DirectionalLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (PointLightsData.SizeInBytes() > PointLightsBuffer->GetSize()) {
		CmdList.DiscardResource(PointLightsBuffer.Get());

		PointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(PointLightsData.CapacityInBytes(),
		                                                                BufferFlag_Default,
		                                                                CEResourceState::VertexAndConstantBuffer,
		                                                                nullptr);
		if (!PointLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (PointLightsPosRad.SizeInBytes() > PointLightsPosRadBuffer->GetSize()) {
		CmdList.DiscardResource(PointLightsPosRadBuffer.Get());

		PointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
			PointLightsPosRad.CapacityInBytes(), BufferFlag_Default, CEResourceState::VertexAndConstantBuffer, nullptr);
		if (!PointLightsPosRadBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (ShadowCastingPointLightsData.SizeInBytes() > ShadowCastingPointLightsBuffer->GetSize()) {
		CmdList.DiscardResource(ShadowCastingPointLightsBuffer.Get());

		ShadowCastingPointLightsBuffer = CastGraphicsManager()->CreateConstantBuffer(
			ShadowCastingPointLightsData.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr);
		if (!ShadowCastingPointLightsBuffer) {
			CEDebug::DebugBreak();
		}
	}

	if (ShadowCastingPointLightsPosRad.SizeInBytes() > ShadowCastingPointLightsPosRadBuffer->GetSize()) {
		CmdList.DiscardResource(ShadowCastingPointLightsPosRadBuffer.Get());

		ShadowCastingPointLightsPosRadBuffer = CastGraphicsManager()->CreateConstantBuffer(
			ShadowCastingPointLightsPosRad.CapacityInBytes(),
			BufferFlag_Default,
			CEResourceState::VertexAndConstantBuffer,
			nullptr);
		if (!ShadowCastingPointLightsPosRadBuffer) {
			CEDebug::DebugBreak();
		}
	}

	CmdList.TransitionBuffer(DirectionalLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                         CEResourceState::CopyDest);
	CmdList.TransitionBuffer(PointLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                         CEResourceState::CopyDest);
	CmdList.TransitionBuffer(PointLightsPosRadBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                         CEResourceState::CopyDest);
	CmdList.TransitionBuffer(ShadowCastingPointLightsBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                         CEResourceState::CopyDest);
	CmdList.TransitionBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), CEResourceState::VertexAndConstantBuffer,
	                         CEResourceState::CopyDest);

	if (!DirectionalLightsData.IsEmpty()) {
		CmdList.UpdateBuffer(DirectionalLightsBuffer.Get(), 0, DirectionalLightsData.SizeInBytes(),
		                     DirectionalLightsData.Data());
	}

	if (!PointLightsData.IsEmpty()) {
		CmdList.UpdateBuffer(PointLightsBuffer.Get(), 0, PointLightsData.SizeInBytes(), PointLightsData.Data());
		CmdList.UpdateBuffer(PointLightsPosRadBuffer.Get(), 0, PointLightsPosRad.SizeInBytes(),
		                     PointLightsPosRad.Data());
	}

	if (!ShadowCastingPointLightsData.IsEmpty()) {
		CmdList.UpdateBuffer(ShadowCastingPointLightsBuffer.Get(), 0, ShadowCastingPointLightsData.SizeInBytes(),
		                     ShadowCastingPointLightsData.Data());
		CmdList.UpdateBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), 0,
		                     ShadowCastingPointLightsPosRad.SizeInBytes(), ShadowCastingPointLightsPosRad.Data());
	}

	CmdList.TransitionBuffer(DirectionalLightsBuffer.Get(), CEResourceState::CopyDest,
	                         CEResourceState::VertexAndConstantBuffer);
	CmdList.TransitionBuffer(PointLightsBuffer.Get(), CEResourceState::CopyDest,
	                         CEResourceState::VertexAndConstantBuffer);
	CmdList.TransitionBuffer(PointLightsPosRadBuffer.Get(), CEResourceState::CopyDest,
	                         CEResourceState::VertexAndConstantBuffer);
	CmdList.TransitionBuffer(ShadowCastingPointLightsBuffer.Get(), CEResourceState::CopyDest,
	                         CEResourceState::VertexAndConstantBuffer);
	CmdList.TransitionBuffer(ShadowCastingPointLightsPosRadBuffer.Get(), CEResourceState::CopyDest,
	                         CEResourceState::VertexAndConstantBuffer);

	INSERT_DEBUG_CMDLIST_MARKER(CmdList, "End Update Lights");
}

void ConceptEngine::Graphics::Main::Rendering::CEBaseLightSetup::Release() {
	PointLightsPosRadBuffer.Reset();
	PointLightsBuffer.Reset();
	ShadowCastingPointLightsBuffer.Reset();
	ShadowCastingPointLightsPosRadBuffer.Reset();
	DirectionalLightsBuffer.Reset();

	PointLightShadowMaps.Reset();

	for (auto& DSVCube : PointLightShadowMapDSVs) {
		for (uint32 i = 0; i < 6; i++) {
			DSVCube[i].Reset();
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
