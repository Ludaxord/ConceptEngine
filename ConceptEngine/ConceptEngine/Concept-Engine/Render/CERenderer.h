#pragma once
#include "Scene/CEScene.h"
#include "../Core/Common/CEEvents.h"
#include "../Graphics/Main/RenderLayer/CECommandList.h"
#include "../Graphics/Main/Rendering/CEDeferredRenderer.h"

namespace ConceptEngine::Render {
	class CERenderer {
	public:
		bool Create();
		void Release();
		void Update(const Scene::CEScene& scene);

		void PerformFrustumCulling(const Scene::CEScene& scene);
		void PerformFXAA(Graphics::Main::RenderLayer::CECommandList& commandList);
		void PerformBackBufferBlit(Graphics::Main::RenderLayer::CECommandList& commandList);

		void RenderDebugInterface();

	private:
		void OnWindowResize(const Core::Common::CEWindowResizeEvent& Event);

		bool CreateBoundingBoxDebugPass();
		bool CreateAA();
		bool CreateShadingImage();

		void ResizeResources(uint32 width, uint32 height);

		Graphics::Main::RenderLayer::CECommandList CommandList;

		Graphics::Main::Rendering::CEDeferredRenderer DeferredRenderer;
	};
}

extern ConceptEngine::Render::CERenderer Renderer;
