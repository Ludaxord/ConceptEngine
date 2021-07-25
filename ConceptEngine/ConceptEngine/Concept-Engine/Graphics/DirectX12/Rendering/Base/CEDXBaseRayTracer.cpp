#include "CEDXBaseRayTracer.h"

#include "../../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../../Core/Debug/CEDebug.h"
#include "../../../../Core/Debug/CEProfiler.h"
#include "../../../../Render/Scene/CEScene.h"
#include "../../../Main/Common/CEMaterial.h"
#include "../../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering::Base;

bool CEDXBaseRayTracer::Create(Main::Rendering::CEFrameResources& Resources) {
	CEArray<uint8> Code;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/RayGen.hlsl"), "RayGen", nullptr, CEShaderStage::RayGen,
	                                     CEShaderModel::SM_6_3, Code)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayGenShader = CastGraphicsManager()->CreateRayGenShader(Code);
	if (!RayGenShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RayGenShader->SetName("RayGenShader");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ClosestHit.hlsl"), "ClosestHit", nullptr,
	                                     CEShaderStage::RayClosestHit, CEShaderModel::SM_6_3, Code)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayClosestHitShader = CastGraphicsManager()->CreateRayClosestHitShader(Code);
	if (!RayClosestHitShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RayClosestHitShader->SetName("RayClosestHitShader");
	}

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders//Miss.hlsl"), "Miss", nullptr, CEShaderStage::RayMiss,
	                                     CEShaderModel::SM_6_3, Code)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayMissShader = CastGraphicsManager()->CreateRayMissShader(Code);
	if (!RayMissShader) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		RayMissShader->SetName("RayMissShader");
	}

	CERayTracingPipelineStateCreateInfo CreateInfo;
	CreateInfo.RayGen = RayGenShader.Get();
	CreateInfo.ClosestHitShaders = {RayClosestHitShader.Get()};
	CreateInfo.MissShaders = {RayMissShader.Get()};
	CreateInfo.HitGroups = {CERayTracingHitGroup("HitGroup", nullptr, RayClosestHitShader.Get())};
	CreateInfo.MaxRecursionDepth = 4;
	CreateInfo.MaxAttributeSizeInBytes = sizeof(CERayIntersectionAttributes);
	CreateInfo.MaxPayloadSizeInBytes = sizeof(CERayPayload);

	Pipeline = CastGraphicsManager()->CreateRayTracingPipelineState(CreateInfo);
	if (!Pipeline) {
		CEDebug::DebugBreak();
		return false;
	}

	uint32 Width = Resources.MainWindowViewport->GetWidth();
	uint32 Height = Resources.MainWindowViewport->GetHeight();
	Resources.RTOutput = CastGraphicsManager()->CreateTexture2D(Resources.RTOutputFormat, Width, Height, 1, 1,
	                                                            TextureFlags_RWTexture,
	                                                            CEResourceState::UnorderedAccess, nullptr);
	if (!Resources.RTOutput) {
		CEDebug::DebugBreak();
		return false;
	}
	else {
		Resources.RTOutput->SetName("RayTracing Output");
	}

	return true;
}

void CEDXBaseRayTracer::PreRender(CECommandList& CmdList,
                                  Main::Rendering::CEFrameResources& Resources,
                                  const Render::Scene::CEScene& Scene) {
	TRACE_SCOPE("Gather Instances");

	Resources.RTGeometryInstances.Clear();

	CESamplerState* Sampler = nullptr;

	for (const Main::Rendering::CEMeshDrawCommand& Cmd : Scene.GetMeshDrawCommands()) {
		Main::Common::CEMaterial* Mat = Cmd.Material;
		if (Cmd.Material->HasAlphaMask()) {
			continue;
		}

		uint32 AlbedoIndex = Resources.RTMaterialTextureCache.Add(Mat->AlbedoMap->GetShaderResourceView());
		Resources.RTMaterialTextureCache.Add(Mat->NormalMap->GetShaderResourceView());
		Resources.RTMaterialTextureCache.Add(Mat->RoughnessMap->GetShaderResourceView());
		Resources.RTMaterialTextureCache.Add(Mat->HeightMap->GetShaderResourceView());
		Resources.RTMaterialTextureCache.Add(Mat->MetallicMap->GetShaderResourceView());
		Resources.RTMaterialTextureCache.Add(Mat->AOMap->GetShaderResourceView());
		Sampler = Mat->GetMaterialSampler();

		const XMFLOAT3X4 TinyTransform = Cmd.CurrentActor->GetTransform().GetTinyMatrix().Native;
		uint32 HitGroupIndex = 0;

		auto HitGroupIndexPair = Resources.RTMeshToHitGroupIndex.find(Cmd.Mesh);
		if (HitGroupIndexPair == Resources.RTMeshToHitGroupIndex.end()) {
			HitGroupIndex = Resources.RTHitGroupResources.Size();
			Resources.RTMeshToHitGroupIndex[Cmd.Mesh] = HitGroupIndex;

			CERayTracingShaderResources HitGroupResources;
			HitGroupResources.Identifier = "HitGroup";
			if (Cmd.Mesh->VertexBufferSRV) {
				HitGroupResources.AddShaderResourceView(Cmd.Mesh->VertexBufferSRV.Get());
			}
			if (Cmd.Mesh->IndexBufferSRV) {
				HitGroupResources.AddShaderResourceView(Cmd.Mesh->IndexBufferSRV.Get());
			}

			Resources.RTHitGroupResources.EmplaceBack(HitGroupResources);
		}
		else {
			HitGroupIndex = HitGroupIndexPair->second;
		}

		CERayTracingGeometryInstance Instance;
		Instance.Instance = MakeSharedRef<CERayTracingGeometry>(Cmd.Geometry);
		Instance.Flags = RayTracingInstanceFlags_None;
		Instance.HitGroupIndex = HitGroupIndex;
		Instance.InstanceIndex = AlbedoIndex;
		Instance.Mask = 0xff;
		Instance.Transform = TinyTransform;
		Resources.RTGeometryInstances.EmplaceBack(Instance);
	}

	if (!Resources.RTScene) {
		Resources.RTScene = CastGraphicsManager()->CreateRayTracingScene(
			RayTracingStructureBuildFlag_None, Resources.RTGeometryInstances.Data(),
			Resources.RTGeometryInstances.Size());
		if (Resources.RTScene) {
			Resources.RTScene->SetName("RayTracingScene");
		}
	}
	else {
		CmdList.BuildRayTracingScene(Resources.RTScene.Get(), Resources.RTGeometryInstances.Data(),
		                             Resources.RTGeometryInstances.Size(), false);
	}

	Resources.GlobalResources.Reset();
	Resources.GlobalResources.AddUnorderedAccessView(Resources.RTOutput->GetUnorderedAccessView());
	Resources.GlobalResources.AddConstantBuffer(Resources.CameraBuffer.Get());
	Resources.GlobalResources.AddSamplerState(Resources.GBufferSampler.Get());
	Resources.GlobalResources.AddSamplerState(Sampler);
	Resources.GlobalResources.AddShaderResourceView(Resources.RTScene->GetShaderResourceView());
	Resources.GlobalResources.AddShaderResourceView(Resources.Skybox->GetShaderResourceView());
	Resources.GlobalResources.AddShaderResourceView(Resources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView());
	Resources.GlobalResources.AddShaderResourceView(Resources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView());

	for (uint32 i = 0; i < Resources.RTMaterialTextureCache.Size(); i++) {
		Resources.GlobalResources.AddShaderResourceView(Resources.RTMaterialTextureCache.Get(i));
	}

	Resources.RayGenLocalResources.Reset();
	Resources.RayGenLocalResources.Identifier = "RayGen";

	Resources.MissLocalResources.Reset();
	Resources.MissLocalResources.Identifier = "Miss";

	// TODO: NO MORE BINDINGS CAN BE BOUND BEFORE DISPATCH RAYS, FIX THIS
	CmdList.SetRayTracingBindings(
		Resources.RTScene.Get(),
		Pipeline.Get(),
		&Resources.GlobalResources,
		&Resources.RayGenLocalResources,
		&Resources.MissLocalResources,
		Resources.RTHitGroupResources.Data(),
		Resources.RTHitGroupResources.Size());

	uint32 Width = Resources.RTOutput->GetWidth();
	uint32 Height = Resources.RTOutput->GetHeight();
	CmdList.DispatchRays(Resources.RTScene.Get(), Pipeline.Get(), Width, Height, 1);

	CmdList.UnorderedAccessTextureBarrier(Resources.RTOutput.Get());

	Resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(Resources.RTOutput->GetShaderResourceView()),
		Resources.RTOutput,
		CEResourceState::UnorderedAccess,
		CEResourceState::UnorderedAccess);
}
