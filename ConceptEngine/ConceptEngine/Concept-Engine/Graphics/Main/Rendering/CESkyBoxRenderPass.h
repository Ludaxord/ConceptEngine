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

		virtual bool Create(CEFrameResources& resources,
		                    const CEPanoramaConfig& panoramaConfig) = 0;
		void Release();

		virtual void Render(RenderLayer::CECommandList& commandList, const CEFrameResources& frameResources,
		                    const Render::Scene::CEScene& scene) = 0;

	protected:
		CERef<RenderLayer::CEGraphicsPipelineState> PipelineState;
		CERef<RenderLayer::CEVertexShader> SkyboxVertexShader;
		CERef<RenderLayer::CEPixelShader> SkyboxPixelShader;
		CERef<RenderLayer::CEVertexBuffer> SkyboxVertexBuffer;
		CERef<RenderLayer::CEIndexBuffer> SkyboxIndexBuffer;
		CERef<RenderLayer::CESamplerState> SkyboxSampler;

		CEMeshData SkyboxMesh;
	};
}
