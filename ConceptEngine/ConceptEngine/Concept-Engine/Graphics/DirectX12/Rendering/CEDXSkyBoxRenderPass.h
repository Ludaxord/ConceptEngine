#pragma once

#include "../../Main/Rendering/CESkyBoxRenderPass.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {

	class CEDXSkyBoxRenderPass : public Main::Rendering::CESkyBoxRenderPass {

	public:
		bool Create( Main::Rendering::CEFrameResources& resources,
                                  const Main::Rendering::CEPanoramaConfig& panoramaConfig) override;
		void Render(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEFrameResources& frameResources, const Render::Scene::CEScene& scene) override;
	private:
	};
}
