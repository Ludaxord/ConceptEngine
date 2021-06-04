#pragma once

#include "../../Main/Rendering/CEScreenSpaceOcclusionRenderer.h"

#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {

	class CEDXScreenSpaceOcclusionRenderer : public Main::Rendering::CEScreenSpaceOcclusionRenderer {

	public:
		bool Create(const Main::Rendering::CEFrameResources& resources) override;
		void Render(Main::RenderLayer::CECommandList& commandList,
		            Main::Rendering::CEFrameResources& frameResources) override;
		bool ResizeResources(Main::Rendering::CEFrameResources& resources) override;
	private:
		bool CreateRenderTarget(Main::Rendering::CEFrameResources& frameResources);
	};
}
