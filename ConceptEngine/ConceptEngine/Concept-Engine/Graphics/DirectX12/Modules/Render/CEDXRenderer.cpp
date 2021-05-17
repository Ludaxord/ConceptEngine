#include "CEDXRenderer.h"

#include "../../../Main/Managers/CEGraphicsManager.h"
#include "../../../../Core/Application/CECore.h"

using namespace ConceptEngine::Graphics::DirectX12::Modules::Render;
using namespace ConceptEngine::Render::Scene;
using namespace ConceptEngine::Graphics::Main::RenderLayer;
using namespace ConceptEngine::Core::Common;

ConceptEngine::Render::CERenderer* Renderer = new CEDXRenderer();

struct CEDXCameraBufferDesc {

};

bool CEDXRenderer::Create() {
	if (!CERenderer::Create()) {
		return false;
	}
	return false;
}

void CEDXRenderer::Release() {
	CERenderer::Release();
}

void CEDXRenderer::Update(const CEScene& scene) {
	CERenderer::Update(scene);
}

void CEDXRenderer::PerformFrustumCulling(const CEScene& scene) {
	CERenderer::PerformFrustumCulling(scene);
}

void CEDXRenderer::PerformFXAA(Main::RenderLayer::CECommandList& commandList) {
	CERenderer::PerformFXAA(commandList);
}

void CEDXRenderer::PerformBackBufferBlit(Main::RenderLayer::CECommandList& commandList) {
	CERenderer::PerformBackBufferBlit(commandList);
}

void CEDXRenderer::RenderDebugInterface() {
	CERenderer::RenderDebugInterface();
}

void CEDXRenderer::OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) {
}

bool CEDXRenderer::CreateBoundingBoxDebugPass() {
	return false;
}

bool CEDXRenderer::CreateAA() {
	return false;
}

bool CEDXRenderer::CreateShadingImage() {
	return false;
}

void CEDXRenderer::ResizeResources(uint32 width, uint32 height) {
}

CERef<CEConstantBuffer> CEDXRenderer::CreateConstantBuffer(uint32 Flags, CEResourceState InitialState,
                                                           const CEResourceData* InitialData) {
	return dynamic_cast<Main::Managers::CEGraphicsManager*>(
		Core::Application::CECore::GetGraphics()
		->GetManager(CEManagerType::GraphicsManager)
	)->CreateConstantBuffer<CEDXCameraBufferDesc>(
		BufferFlag_Default,
		CEResourceState::Common,
		nullptr
	);
}

CEInputLayoutStateCreateInfo CEDXRenderer::CreateInputLayoutCreateInfo() {
	CEInputLayoutStateCreateInfo InputLayout = {
		{"POSITION", 0, CEFormat::R32G32B32_Float, 0, 0, CEInputClassification::Vertex, 0},
		{"NORMAL", 0, CEFormat::R32G32B32_Float, 0, 12, CEInputClassification::Vertex, 0},
		{"TANGENT", 0, CEFormat::R32G32B32_Float, 0, 24, CEInputClassification::Vertex, 0},
		{"TEXCOORD", 0, CEFormat::R32G32_Float, 0, 36, CEInputClassification::Vertex, 0}
	};

	return InputLayout;
}
