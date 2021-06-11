#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CERayTracer {
	public:
		CERayTracer() = default;
		virtual ~CERayTracer() = default;

		virtual bool Create(CEFrameResources& resources) = 0;

		//TODO: Implement
		void Release() {
			
		};

		virtual void PreRender(RenderLayer::CECommandList& commandList, CEFrameResources& resources,
		                       const Render::Scene::CEScene& scene) = 0;

	protected:
		CERef<RenderLayer::CERayTracingPipelineState> Pipeline;
		CERef<RenderLayer::CERayGenShader> RayGenShader;
		CERef<RenderLayer::CERayMissShader> RayMissShader;
		CERef<RenderLayer::CERayClosestHitShader> RayClosestHitShader;
	};
}
