#include "CEDXRayTracer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"
#include "../../../Core/Debug/CEProfiler.h"
#include "../../Main/Common/CEMaterial.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

bool CEDXRayTracer::Create(Main::Rendering::CEFrameResources& resources) {
	Core::Containers::CEArray<uint8> shaderCode;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/RayGen.hlsl", "RayGen", nullptr, CEShaderStage::RayGen,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	RayGenShader = CastGraphicsManager()->CreateRayGenShader(shaderCode);
	if (!RayGenShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	RayGenShader->SetName("Ray Gen Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/ClosestHit.hlsl", "ClosestHit", nullptr,
	                                     CEShaderStage::RayClosestHit, CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	RayClosestHitShader = CastGraphicsManager()->CreateRayClosestHitShader(shaderCode);
	if (!RayClosestHitShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	RayClosestHitShader->SetName("Ray Closest Hit Shader");

	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/Miss.hlsl", "Miss", nullptr, CEShaderStage::RayMiss,
	                                     CEShaderModel::SM_6_0, shaderCode)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	RayMissShader = CastGraphicsManager()->CreateRayMissShader(shaderCode);
	if (!RayMissShader) {
		Core::Debug::CEDebug::DebugBreak();
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
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	uint32 width = resources.MainWindowViewport->GetWidth();
	uint32 height = resources.MainWindowViewport->GetHeight();
	resources.RTOutput = CastGraphicsManager()->CreateTexture2D(resources.RTOutputFormat, width, height, 1, 1,
	                                                            TextureFlags_RWTexture,
	                                                            CEResourceState::UnorderedAccess, nullptr);
	
	if (!resources.RTOutput) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	resources.RTOutput->SetName("Ray Tracing Output");

	return true;
}

void CEDXRayTracer::PreRender(Main::RenderLayer::CECommandList& commandList,
                              Main::Rendering::CEFrameResources& resources,
                              const Render::Scene::CEScene& scene) {

}
