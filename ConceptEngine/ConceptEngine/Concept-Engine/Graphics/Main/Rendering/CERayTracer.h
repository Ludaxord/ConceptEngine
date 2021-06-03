#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CERayTracer {
	public:
		CERayTracer() = default;
		virtual ~CERayTracer() = default;

		virtual bool Create(const CEFrameResources& resources) = 0;
		void Release();

		virtual void PreRender(RenderLayer::CECommandList& commandList, CEFrameResources& resources,
		                       const Render::Scene::CEScene& scene) = 0;

	protected:
		Core::Common::CERef<RenderLayer::CERayTracingPipelineState> Pipeline;
		Core::Common::CERef<RenderLayer::CERayGenShader> RayGenShader;
		Core::Common::CERef<RenderLayer::CERayMissShader> RayMissShader;
		Core::Common::CERef<RenderLayer::CERayClosestHitShader> RayClosestHitShader;
	};
}
