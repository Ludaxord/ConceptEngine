#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEForwardRenderer {
	public:
		CEForwardRenderer() = default;
		~CEForwardRenderer() = default;

		bool Create(const CEFrameResources& resources);
		void Release();

		void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		            const CELightSetup& lightSetup);

	private:
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> VertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> PixelShader;
	};
}
