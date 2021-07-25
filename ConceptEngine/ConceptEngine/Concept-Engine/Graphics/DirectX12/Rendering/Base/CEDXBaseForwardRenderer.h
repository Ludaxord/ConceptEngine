#pragma once

#include "../../../Main/Rendering/CEForwardRenderer.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering::Base {

	class CEDXBaseForwardRenderer : public Main::Rendering::CEForwardRenderer {

	public:
		bool Create(const Main::Rendering::CEFrameResources& resources) override;
		void Render(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEFrameResources& frameResources,
			const Main::Rendering::CELightSetup& lightSetup) override;
		void Render(Main::RenderLayer::CECommandList& commandList,
			const Main::Rendering::CEFrameResources& frameResources,
			const Main::Rendering::CEBaseLightSetup& lightSetup) override;
	private:
	};
}
