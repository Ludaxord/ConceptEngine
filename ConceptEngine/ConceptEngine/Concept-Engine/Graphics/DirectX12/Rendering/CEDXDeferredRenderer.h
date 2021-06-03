#pragma once

#include "../../Main/Rendering/CEDeferredRenderer.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {
	class CEDXDeferredRenderer : public Main::Rendering::CEDeferredRenderer {
	public:
		bool Create(Main::Rendering::CEFrameResources& FrameResources) override;
		void RenderPrePass(Main::RenderLayer::CECommandList& commandList,
		                   const Main::Rendering::CEFrameResources& frameResources) override;
		void RenderBasePass(Main::RenderLayer::CECommandList& commandList,
		                    const Main::Rendering::CEFrameResources& frameResources) override;
		void RenderDeferredTiledLightPass(Main::RenderLayer::CECommandList& commandList,
		                                  const Main::Rendering::CEFrameResources& frameResources,
		                                  const Main::Rendering::CELightSetup& lightSetup) override;
		bool ResizeResources(Main::Rendering::CEFrameResources& resources) override;

	private:
		bool CreateGBuffer(Main::Rendering::CEFrameResources& frameResources);
	};
}
