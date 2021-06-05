#pragma once
#include "CEFrameResources.h"

namespace ConceptEngine::Graphics::Main::Rendering {
	
	struct CEPanoramaConfig {
		std::string SourceFile;

		bool IsDirty = false;

		bool Update(std::string SourceFile) {
			return true;
		}
	};

	class CESkyBoxRenderPass {
	public:
		CESkyBoxRenderPass() = default;
		virtual ~CESkyBoxRenderPass() = default;

		virtual bool Create(const CEFrameResources& resources,
		                    const CEPanoramaConfig& panoramaConfig) = 0;
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
