#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEShadowMapRenderer {
	public:
		bool Create(const CELightSetup& lightSetup, const CEFrameResources& resources);
		void Release();

		void RenderPointLightShadows(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                             const Render::Scene::CEScene& scene);
		void RenderDirectionalLightShadows(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                                   const Render::Scene::CEScene& scene);

	private:
		bool CreateShadowMaps(CELightSetup& frameResources);

		Core::Common::CERef<RenderLayer::CEConstantBuffer> PerShadowMapBuffer;

		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> DirLightPipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> DirLightShader;
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PointLightPipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> PointLightVertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> PointLightPixelShader;

		bool UpdateDirLight = true;
		bool UpdatePointLight = true;
		uint64 DirLightFrame = 0;
		uint64 PointLightFrame = 0;
	};
}
