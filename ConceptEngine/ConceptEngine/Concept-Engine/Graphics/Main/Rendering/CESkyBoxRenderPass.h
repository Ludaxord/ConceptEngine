#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	class CESkyBoxRenderPass {
	public:
		CESkyBoxRenderPass() = default;
		virtual ~CESkyBoxRenderPass() = default;

		virtual bool Create(const CEFrameResources& resources) = 0;
		void Release();

		virtual void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		            const Render::Scene::CEScene& scene) = 0;

	protected:
		Core::Common::CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		Core::Common::CERef<RenderLayer::CEVertexShader> SkyboxVertexShader;
		Core::Common::CERef<RenderLayer::CEPixelShader> SkyboxPixelShader;
		Core::Common::CERef<RenderLayer::CEVertexBuffer> SkyboxVertexBuffer;
		Core::Common::CERef<RenderLayer::CEIndexBuffer> SkyboxIndexBuffer;
		Core::Common::CERef<RenderLayer::CESamplerState> SkyboxSampler;

		CEMeshData SkyboxMesh;
	};
}
