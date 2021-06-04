#pragma once

#include "../../Main/Rendering/CEScreenSpaceOcclusionRenderer.h"

#include "../../../Core/Platform/Generic/Debug/CEConsoleVariable.h"
#include "../../../Core/Platform/Generic/Debug/CETypedConsole.h"

//Resources: https://culturedvultures.com/screen-space-ambient-occlusion-ssao/
//https://www.gamedesigning.org/learn/ambient-occlusion/
//https://software.intel.com/content/www/us/en/develop/articles/adaptive-screen-space-ambient-occlusion.html

namespace ConceptEngine::Graphics::DirectX12::Rendering {

	class CEDXScreenSpaceOcclusionRenderer : public Main::Rendering::CEScreenSpaceOcclusionRenderer {

	public:
		bool Create( Main::Rendering::CEFrameResources& resources) override;
		void Render(Main::RenderLayer::CECommandList& commandList,
		            Main::Rendering::CEFrameResources& frameResources) override;
		bool ResizeResources(Main::Rendering::CEFrameResources& resources) override;
	private:
		bool CreateRenderTarget(Main::Rendering::CEFrameResources& frameResources);
	};
}
