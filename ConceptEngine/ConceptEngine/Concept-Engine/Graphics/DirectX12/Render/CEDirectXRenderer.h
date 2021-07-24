#pragma once

#include "../../../Render/CERenderer.h"

#include "../../../Render/Scene/CEFrustum.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Render {
	class CEDirectXRenderer : public ConceptEngine::Render::CERenderer {
	public:
		bool Create() override;
		void Release() override;
		void Update(const ConceptEngine::Render::Scene::CEScene& scene) override;
		
		void PerformFrustumCulling(const ConceptEngine::Render::Scene::CEScene& scene) override;
		void PerformFXAA(Main::RenderLayer::CECommandList& commandList) override;
		void PerformBackBufferBlit(Main::RenderLayer::CECommandList& commandList) override;
		void PerformAABBDebugPass(Main::RenderLayer::CECommandList& commandList) override;
		
		void RenderDebugInterface() override;
	private:
		void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) override;
		bool CreateBoundingBoxDebugPass() override;
		bool CreateAA() override;
		bool CreateShadingImage() override;
		void ResizeResources(uint32 width, uint32 height) override;
		const Main::RenderLayer::CEInputLayoutStateCreateInfo& CreateInputLayoutCreateInfo() override;
	};
}
