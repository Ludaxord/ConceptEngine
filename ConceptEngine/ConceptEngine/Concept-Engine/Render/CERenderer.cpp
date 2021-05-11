#include "CERenderer.h"

using namespace ConceptEngine::Render;

bool CERenderer::Create() {
	return true;
}

void CERenderer::Release() {
}

void CERenderer::Update(const Scene::CEScene& scene) {
}

void CERenderer::PerformFrustumCulling(const Scene::CEScene& scene) {
}

void CERenderer::PerformFXAA(Graphics::Main::RenderLayer::CECommandList& commandList) {
}

void CERenderer::PerformBackBufferBlit(Graphics::Main::RenderLayer::CECommandList& commandList) {
}

void CERenderer::RenderDebugInterface() {
}

void CERenderer::OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) {
}

bool CERenderer::CreateBoundingBoxDebugPass() {
	return false;
}

bool CERenderer::CreateAA() {
	return false;
}

bool CERenderer::CreateShadingImage() {
	return false;
}

void CERenderer::ResizeResources(uint32 width, uint32 height) {
}
