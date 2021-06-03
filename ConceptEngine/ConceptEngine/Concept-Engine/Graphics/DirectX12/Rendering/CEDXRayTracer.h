#pragma once

#include "../../Main/Rendering/CERayTracer.h"

namespace ConceptEngine::Graphics::DirectX12::Rendering {

	class CEDXRayTracer : public Main::Rendering::CERayTracer {

	public:
		bool Create(const Main::Rendering::CEFrameResources& resources) override;
		void PreRender(Main::RenderLayer::CECommandList& commandList, Main::Rendering::CEFrameResources& resources,
		               const Render::Scene::CEScene& scene) override;
	private:
	};
}
