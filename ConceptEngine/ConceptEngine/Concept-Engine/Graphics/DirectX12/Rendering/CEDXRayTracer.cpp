#include "CEDXRayTracer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../../Render/Scene/CEScene.h"
#include "../../Main/Common/CEMaterial.h"
#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXRayTracer::Create(Main::Rendering::CEFrameResources& resources) {
	CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/RayGen.hlsl"), "RayGen", nullptr, CEShaderStage::RayGen,
	                                     CEShaderModel::SM_6_3, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayGenShader = CastGraphicsManager()->CreateRayGenShader(shaderCode);
	if (!RayGenShader) {
		CEDebug::DebugBreak();
		return false;
	}

	RayGenShader->SetName("Ray Gen Shader");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/ClosestHit.hlsl"), "ClosestHit", nullptr,
	                                     CEShaderStage::RayClosestHit, CEShaderModel::SM_6_3, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayClosestHitShader = CastGraphicsManager()->CreateRayClosestHitShader(shaderCode);
	if (!RayClosestHitShader) {
		CEDebug::DebugBreak();
		return false;
	}

	RayClosestHitShader->SetName("Ray Closest Hit Shader");

	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/Miss.hlsl"), "Miss", nullptr, CEShaderStage::RayMiss,
	                                     CEShaderModel::SM_6_3, shaderCode)) {
		CEDebug::DebugBreak();
		return false;
	}

	RayMissShader = CastGraphicsManager()->CreateRayMissShader(shaderCode);
	if (!RayMissShader) {
		CEDebug::DebugBreak();
		return false;
	}

	RayMissShader->SetName("Ray Miss Shader");

	CERayTracingPipelineStateCreateInfo createInfo;
	createInfo.RayGen = RayGenShader.Get();
	createInfo.ClosestHitShaders = {RayClosestHitShader.Get()};
	createInfo.MissShaders = {RayMissShader.Get()};
	createInfo.HitGroups = {CERayTracingHitGroup("HitGroup", nullptr, RayClosestHitShader.Get())};
	createInfo.MaxRecursionDepth = 4;
	createInfo.MaxAttributeSizeInBytes = sizeof(CERayIntersectionAttributes);
	createInfo.MaxPayloadSizeInBytes = sizeof(CERayPayload);

	Pipeline = CastGraphicsManager()->CreateRayTracingPipelineState(createInfo);
	if (!Pipeline) {
		CEDebug::DebugBreak();
		return false;
	}

	uint32 width = resources.MainWindowViewport->GetWidth();
	uint32 height = resources.MainWindowViewport->GetHeight();
	resources.RTOutput = CastGraphicsManager()->CreateTexture2D(resources.RTOutputFormat, width, height, 1, 1,
	                                                            TextureFlags_RWTexture,
	                                                            CEResourceState::UnorderedAccess, nullptr);

	if (!resources.RTOutput) {
		CEDebug::DebugBreak();
		return false;
	}

	resources.RTOutput->SetName("Ray Tracing Output");

	return true;
}

void CEDXRayTracer::PreRender(Main::RenderLayer::CECommandList& commandList,
                              Main::Rendering::CEFrameResources& resources,
                              const Render::Scene::CEScene& scene) {
	TRACE_SCOPE("Gather Instances");

	resources.RTGeometryInstances.Clear();

	CESamplerState* sampler = nullptr;

	for (const Main::Rendering::CEMeshDrawCommand& command : scene.GetMeshDrawCommands()) {
		Main::Common::CEMaterial* material = command.Material;
		if (command.Material->HasAlphaMask()) {
			continue;
		}

		uint32 albedoIndex = resources.RTMaterialTextureCache.Add(material->AlbedoMap->GetShaderResourceView());
		resources.RTMaterialTextureCache.Add(material->NormalMap->GetShaderResourceView());
		resources.RTMaterialTextureCache.Add(material->RoughnessMap->GetShaderResourceView());
		resources.RTMaterialTextureCache.Add(material->HeightMap->GetShaderResourceView());
		resources.RTMaterialTextureCache.Add(material->MetallicMap->GetShaderResourceView());
		resources.RTMaterialTextureCache.Add(material->AOMap->GetShaderResourceView());
		sampler = material->GetMaterialSampler();

		auto tinyTransform = command.CurrentActor->GetTransform().GetTinyMatrix();
		uint32 hitGroupIndex = 0;

		auto hitGroupIndexPair = resources.RTMeshToHitGroupIndex.find(command.Mesh);
		if (hitGroupIndexPair == resources.RTMeshToHitGroupIndex.end()) {

			hitGroupIndex = resources.RTHitGroupResources.Size();
			resources.RTMeshToHitGroupIndex[command.Mesh] = hitGroupIndex;

			CERayTracingShaderResources hitGroupResources;
			hitGroupResources.Identifier = "HitGroup";
			if (command.Mesh->VertexBufferSRV) {
				hitGroupResources.AddShaderResourceView(command.Mesh->VertexBufferSRV.Get());
			}

			if (command.Mesh->IndexBufferSRV) {
				hitGroupResources.AddShaderResourceView(command.Mesh->IndexBufferSRV.Get());
			}

			resources.RTHitGroupResources.EmplaceBack(hitGroupResources);
		}
		else {
			hitGroupIndex = hitGroupIndexPair->second;
		}

		CERayTracingGeometryInstance instance;
		instance.Instance = MakeSharedRef<CERayTracingGeometry>(command.Geometry);
		instance.Flags = RayTracingInstanceFlags_None;
		instance.HitGroupIndex = hitGroupIndex;
		instance.InstanceIndex = albedoIndex;
		instance.Mask = 0xff;
		instance.Transform = tinyTransform;
		resources.RTGeometryInstances.EmplaceBack(instance);
	}

	if (!resources.RTScene) {
		resources.RTScene = CastGraphicsManager()->CreateRayTracingScene(
			RayTracingStructureBuildFlag_None, resources.RTGeometryInstances.Data(),
			resources.RTGeometryInstances.Size());
		if (resources.RTScene) {
			resources.RTScene->SetName("Ray Tracing Scene");
		}
	}
	else {
		commandList.BuildRayTracingScene(resources.RTScene.Get(), resources.RTGeometryInstances.Data(),
		                                 resources.RTGeometryInstances.Size(), false);
	}

	resources.GlobalResources.Reset();
	resources.GlobalResources.AddUnorderedAccessView(resources.RTOutput->GetUnorderedAccessView());
	resources.GlobalResources.AddConstantBuffer(resources.CameraBuffer.Get());
	resources.GlobalResources.AddSamplerState(resources.GBufferSampler.Get());
	resources.GlobalResources.AddSamplerState(sampler);
	resources.GlobalResources.AddShaderResourceView(resources.RTScene->GetShaderResourceView());
	resources.GlobalResources.AddShaderResourceView(resources.Skybox->GetShaderResourceView());
	resources.GlobalResources.AddShaderResourceView(resources.GBuffer[BUFFER_NORMAL_INDEX]->GetShaderResourceView());
	resources.GlobalResources.AddShaderResourceView(resources.GBuffer[BUFFER_DEPTH_INDEX]->GetShaderResourceView());

	for (uint32 i = 0; i < resources.RTMaterialTextureCache.Size(); i++) {
		resources.GlobalResources.AddShaderResourceView(resources.RTMaterialTextureCache.Get(i));
	}

	resources.RayGenLocalResources.Reset();
	resources.RayGenLocalResources.Identifier = "RayGen";

	resources.MissLocalResources.Reset();
	resources.MissLocalResources.Identifier = "Miss";

	commandList.SetRayTracingBindings(
		resources.RTScene.Get(),
		Pipeline.Get(),
		&resources.GlobalResources,
		&resources.RayGenLocalResources,
		&resources.MissLocalResources,
		resources.RTHitGroupResources.Data(),
		resources.RTHitGroupResources.Size()
	);

	uint32 width = resources.RTOutput->GetWidth();
	uint32 height = resources.RTOutput->GetHeight();
	commandList.DispatchRays(resources.RTScene.Get(), Pipeline.Get(), width, height, 1);

	commandList.UnorderedAccessTextureBarrier(resources.RTOutput.Get());

	resources.DebugTextures.EmplaceBack(
		MakeSharedRef<CEShaderResourceView>(resources.RTOutput->GetShaderResourceView()),
		resources.RTOutput,
		CEResourceState::UnorderedAccess,
		CEResourceState::UnorderedAccess
	);
}
