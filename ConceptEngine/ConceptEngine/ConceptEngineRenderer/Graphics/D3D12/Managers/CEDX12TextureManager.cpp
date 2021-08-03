#include "CEDX12TextureManager.h"

#include "CETextureData.h"
#include "RenderLayer/CEGraphicsManager.h"
#include "RenderLayer/CEShaderCompiler.h"
#include "RenderLayer/PipelineState.h"

CEDX12TextureManager::CEDX12TextureManager(): CETextureManager() {
}

CEDX12TextureManager::~CEDX12TextureManager() {
}

bool CEDX12TextureManager::Create() {
	// Compile and create shader
	CEArray<uint8> Code;
	if (!CEShaderCompiler::CompileFromFile("../ConceptEngineRenderer/Shaders/CubeMapGen.hlsl", "Main", nullptr,
	                                       EShaderStage::Compute, EShaderModel::SM_6_0, Code)) {
		return false;
	}

	GlobalFactoryData.ComputeShader = CreateComputeShader(Code);
	if (!GlobalFactoryData.ComputeShader) {
		CE_LOG_ERROR("Failed to Create CubeMapGen from Compute Shader")
		return false;
	}

	// Create pipeline
	GlobalFactoryData.PanoramaPSO = CreateComputePipelineState(
		ComputePipelineStateCreateInfo(GlobalFactoryData.ComputeShader.Get()));
	if (GlobalFactoryData.PanoramaPSO) {
		CE_LOG_VERBOSE("Create CubeMaps Panorama PSO")
		GlobalFactoryData.PanoramaPSO->SetName("Generate CubeMap RootSignature");
		return true;
	}
	return false;
}

void CEDX12TextureManager::Release() {
	GlobalFactoryData.PanoramaPSO.Reset();
	GlobalFactoryData.ComputeShader.Reset();
}
