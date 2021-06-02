#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CESkyBoxRenderPass {
	public:
		CESkyBoxRenderPass() = default;
		~CESkyBoxRenderPass() = default;

		bool Create(const CEFrameResources& resources);
		void Release();

		void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		            const Render::Scene::CEScene& scene);

	private:
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> SkyboxVertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> SkyboxPixelShader;
		Core::Common::CERef<RenderLayer::CEVertexBuffer> SkyboxVertexBuffer;
		Core::Common::CERef<RenderLayer::CEIndexBuffer> SkyboxIndexBuffer;
		Core::Common::CERef<RenderLayer::CESamplerState> SkyboxSampler;

		CEMeshData SkyboxMesh;
	};
}
