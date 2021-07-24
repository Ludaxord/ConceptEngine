#pragma once
#include "CEFrameResources.h"
#include "CELightSetup.h"
#include "CEBaseLightSetup.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CEShadowMapRenderer {
	public:
		virtual ~CEShadowMapRenderer() = default;
		virtual bool Create( CELightSetup& lightSetup, CEFrameResources& resources) = 0;
		virtual bool Create( CEBaseLightSetup& lightSetup, CEFrameResources& resources) = 0;
		void Release();

		virtual void RenderPointLightShadows(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                             const Render::Scene::CEScene& scene)= 0;
		virtual void RenderDirectionalLightShadows(RenderLayer::CECommandList& commandList, const CELightSetup& lightSetup,
		                                   const Render::Scene::CEScene& scene)= 0;

	protected:
		CERef<RenderLayer::CEConstantBuffer> PerShadowMapBuffer;

		CERef<RenderLayer::CEGraphicsPipelineState> DirLightPipelineState;
		CERef<RenderLayer::CEVertexShader> DirLightShader;
		CERef<RenderLayer::CEGraphicsPipelineState> PointLightPipelineState;
		CERef<RenderLayer::CEVertexShader> PointLightVertexShader;
		CERef<RenderLayer::CEPixelShader> PointLightPixelShader;

		bool UpdateDirLight = true;
		bool UpdatePointLight = true;
		uint64 DirLightFrame = 0;
		uint64 PointLightFrame = 0;
	};
}
