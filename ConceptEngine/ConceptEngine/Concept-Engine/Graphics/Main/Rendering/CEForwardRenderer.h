#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEForwardRenderer {
	public:
		CEForwardRenderer() = default;
		virtual ~CEForwardRenderer() = default;

		virtual bool Create(const CEFrameResources& resources) = 0;

		//TODO:Implement
		void Release() {

		};

		virtual void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		                    const CELightSetup& lightSetup) = 0;

	protected:
		CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		CERef<RenderLayer::CEVertexShader> VertexShader;
		CERef<RenderLayer::CEPixelShader> PixelShader;
	};
}
