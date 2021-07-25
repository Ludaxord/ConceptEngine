#pragma once

#include "../../../Main/Rendering/CEShadowMapRenderer.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering::Base {
	class CEDXBaseShadowMapRenderer : public Main::Rendering::CEShadowMapRenderer {
	public:
		bool Create( Main::Rendering::CELightSetup& lightSetup,
		            Main::Rendering::CEFrameResources& resources) override;
		void RenderPointLightShadows(Main::RenderLayer::CECommandList& commandList,
		                             const Main::Rendering::CELightSetup& lightSetup,
		                             const Render::Scene::CEScene& scene) override;
		void RenderDirectionalLightShadows(Main::RenderLayer::CECommandList& commandList,
		                                   const Main::Rendering::CELightSetup& lightSetup,
		                                   const Render::Scene::CEScene& scene) override;

	private:
		bool CreateShadowMaps(Main::Rendering::CELightSetup& frameResources);
		bool CreateShadowMaps(Main::Rendering::CEBaseLightSetup& frameResources);
	public:
		bool Create(Main::Rendering::CEBaseLightSetup& lightSetup,
			Main::Rendering::CEFrameResources& resources) override;
		void RenderPointLightShadows(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEBaseLightSetup& lightSetup, const Render::Scene::CEScene& scene) override;
		void RenderDirectionalLightShadows(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEBaseLightSetup& lightSetup, const Render::Scene::CEScene& scene) override;
	};
}
