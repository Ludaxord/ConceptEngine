#pragma once

#include "../../../Main/Rendering/CELightProbeRenderer.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering::Base {

	class CEDXBaseLightProbeRenderer : public Main::Rendering::CELightProbeRenderer {

	public:
		bool Create(Main::Rendering::CELightSetup& lightSetup,
		            Main::Rendering::CEFrameResources& frameResources) override;
		void RenderSkyLightProbe(Main::RenderLayer::CECommandList& commandList,
		                         const Main::Rendering::CELightSetup& lightSetup,
		                         const Main::Rendering::CEFrameResources& resources) override;
	private:
		bool CreateSkyLightResources(Main::Rendering::CELightSetup& lightSetup);
		bool CreateSkyLightResources(Main::Rendering::CEBaseLightSetup& lightSetup);
	public:
		bool Create(Main::Rendering::CEBaseLightSetup& lightSetup,
			Main::Rendering::CEFrameResources& frameResources) override;
		void RenderSkyLightProbe(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEBaseLightSetup& lightSetup,
			const Main::Rendering::CEFrameResources& resources) override;
	};
}
