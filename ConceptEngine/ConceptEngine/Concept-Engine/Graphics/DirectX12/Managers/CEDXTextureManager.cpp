#include "CEDXTextureManager.h"

#include "../../../Core/Containers/CEArray.h"

#include "../RenderLayer/CEDXShaderCompiler.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Debug/CEDebug.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Utilities/CEDirectoryUtilities.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;

CEDXTextureManager::CEDXTextureManager() : CETextureManager() {
}

CEDXTextureManager::~CEDXTextureManager() {
}

bool CEDXTextureManager::Create() {
	CEArray<uint8> code;
	if (!ShaderCompiler->CompileFromFile(GetGraphicsContentDirectory("DirectX12/Shaders/CubeMapGen.hlsl"), "Main", nullptr,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     code)) {
		CEDebug::DebugBreak();
		return false;
	}


	Main::MainTextureData.ComputeShader = CastGraphicsManager()->CreateComputeShader(code);

	if (!Main::MainTextureData.ComputeShader) {
		CEDebug::DebugBreak();
		return false;
	}

	Main::MainTextureData.PanoramaPSO = CastGraphicsManager()->CreateComputePipelineState(
		RenderLayer::CEComputePipelineStateCreateInfo(
			Main::MainTextureData.ComputeShader.Get()
		)
	);

	if (Main::MainTextureData.PanoramaPSO) {
		Main::MainTextureData.PanoramaPSO->SetName("Generate Cube Map Root Signature");
		return true;
	}

	return false;

}
