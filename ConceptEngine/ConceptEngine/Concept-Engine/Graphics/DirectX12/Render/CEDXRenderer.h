#pragma once

#include "../../../Render/CERenderer.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Render {
	class CEDXRenderer : public ConceptEngine::Render::CERenderer {
	public:
		bool Create() override;
		void Release() override;
		void Update(const ConceptEngine::Render::Scene::CEScene& scene) override;
		void PerformFrustumCulling(const ConceptEngine::Render::Scene::CEScene& scene) override;
		void PerformFXAA(Main::RenderLayer::CECommandList& commandList) override;
		void PerformBackBufferBlit(Main::RenderLayer::CECommandList& commandList) override;
		void RenderDebugInterface() override;
	private:
		void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event) override;
		bool CreateBoundingBoxDebugPass() override;
		bool CreateAA() override;
		bool CreateShadingImage() override;
		void ResizeResources(uint32 width, uint32 height) override;

		Core::Common::CERef<Main::RenderLayer::CEConstantBuffer> CreateConstantBuffer(
			uint32 Flags,
			Main::RenderLayer::CEResourceState InitialState,
			const Main::RenderLayer::CEResourceData* InitialData
		) override;

		const Main::RenderLayer::CEInputLayoutStateCreateInfo& CreateInputLayoutCreateInfo() override;
	};
}
