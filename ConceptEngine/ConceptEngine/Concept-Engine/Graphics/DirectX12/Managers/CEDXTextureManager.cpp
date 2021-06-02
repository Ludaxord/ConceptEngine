#include "CEDXTextureManager.h"

#include "../../../Core/Containers/CEArray.h"

#include "../RenderLayer/CEDXShaderCompiler.h"

#include "../../../Core/Application/CECore.h"

#include "../../../Core/Debug/CEDebug.h"

using namespace ConceptEngine::Graphics::DirectX12::Managers;

CEDXTextureManager::CEDXTextureManager() : CETextureManager() {
}

CEDXTextureManager::~CEDXTextureManager() {
}

bool CEDXTextureManager::Create() {
	Core::Containers::CEArray<uint8> code;
	if (!ShaderCompiler->CompileFromFile("DirectX12/Shaders/CubeMapGen.hlsl", "Main", nullptr,
	                                     RenderLayer::CEShaderStage::Compute, RenderLayer::CEShaderModel::SM_6_0,
	                                     code)) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}


	Main::MainTextureData.ComputeShader = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->GetManager(Core::Common::CEManagerType::GraphicsManager)
	)->CreateComputeShader(code);

	if (!Main::MainTextureData.ComputeShader) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	Main::MainTextureData.PanoramaPSO = dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()->GetManager(Core::Common::CEManagerType::GraphicsManager)
	)->CreateComputePipelineState(
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
