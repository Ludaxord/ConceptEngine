#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CERayTracer {
	public:
		CERayTracer() = default;
		~CERayTracer() = default;

		bool Create(const CEFrameResources& resources);
		void Release();

		void PreRender(RenderLayer::CECommandList& commandList, CEFrameResources& resources,
		               const Render::Scene::CEScene& scene);

	private:
		Core::Common::CERef<RenderLayer::CERayTracingPipelineState> Pipeline;
		Core::Common::CERef<RenderLayer::CERayGenShader> RayGenShader;
		Core::Common::CERef<RenderLayer::CERayMissShader> RayMissShader;
		Core::Common::CERef<RenderLayer::CERayClosestHitShader> RayClosestHitShader;
	};
}
