#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEForwardRenderer {
	public:
		CEForwardRenderer() = default;
		virtual ~CEForwardRenderer() = default;

		virtual bool Create(const CEFrameResources& resources) = 0;
		void Release();

		virtual void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		                    const CELightSetup& lightSetup) = 0;

	protected:
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> VertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> PixelShader;
	};
}
