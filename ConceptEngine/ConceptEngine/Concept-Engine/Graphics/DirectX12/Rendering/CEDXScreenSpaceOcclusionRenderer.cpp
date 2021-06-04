#include "CEDXScreenSpaceOcclusionRenderer.h"

#include "../../../Core/Platform/Generic/Managers/CECastManager.h"

#include "../../../Core/Debug/CEDebug.h"

using namespace ConceptEngine::Graphics::DirectX12::Rendering;

ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAORadius(0.3f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<float> GSSAOBias(0.03f);
ConceptEngine::Core::Platform::Generic::Debug::CEConsoleVariableEx<int32> GSSAOKernelSize(32);

bool CEDXScreenSpaceOcclusionRenderer::Create(const Main::Rendering::CEFrameResources& resources) {
	return true;
}

void CEDXScreenSpaceOcclusionRenderer::Render(Main::RenderLayer::CECommandList& commandList,
                                              Main::Rendering::CEFrameResources& frameResources) {
}

bool CEDXScreenSpaceOcclusionRenderer::ResizeResources(Main::Rendering::CEFrameResources& resources) {
	return CreateRenderTarget(resources);
}

bool CEDXScreenSpaceOcclusionRenderer::CreateRenderTarget(Main::Rendering::CEFrameResources& frameResources) {
	const uint32 width = frameResources.MainWindowViewport->GetWidth();
	const uint32 height = frameResources.MainWindowViewport->GetHeight();
	const uint32 flags = Main::RenderLayer::TextureFlags_RWTexture;

	frameResources.SSAOBuffer = CastGraphicsManager()->CreateTexture2D(frameResources.SSAOBufferFormat,
	                                                                   width,
	                                                                   height,
	                                                                   1,
	                                                                   1,
	                                                                   flags,
	                                                                   RenderLayer::CEResourceState::Common,
	                                                                   nullptr);
	if (!frameResources.SSAOBuffer) {
		Core::Debug::CEDebug::DebugBreak();
		return false;
	}

	frameResources.SSAOBuffer->SetName("SSAO Buffer");

	return true;
}
