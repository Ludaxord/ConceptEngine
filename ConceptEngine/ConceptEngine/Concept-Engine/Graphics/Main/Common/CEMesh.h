#pragma once

#include "../Managers/CEMeshManager.h"

#include "../../../Render/Scene/CEAABB.h"

#include "../RenderLayer/CECommandList.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CERayTracing.h"


namespace ConceptEngine::Graphics::Main::Common {
	class CEMesh {
	public:
		CEMesh() = default;
		~CEMesh() = default;

		bool Create(const CEMeshData& data);

		bool BuildAccelerationStructure(ConceptEngine::Graphics::Main::RenderLayer::CECommandList& commandList);

		static std::shared_ptr<CEMesh> Make(const CEMeshData& data);

		void CreateBoundingBox(const CEMeshData& data);

		CERef<ConceptEngine::Graphics::Main::RenderLayer::CEVertexBuffer> VertexBuffer;
		CERef<ConceptEngine::Graphics::Main::RenderLayer::CEShaderResourceView> VertexBufferSRV;
		CERef<ConceptEngine::Graphics::Main::RenderLayer::CEIndexBuffer> IndexBuffer;
		CERef<ConceptEngine::Graphics::Main::RenderLayer::CEShaderResourceView> IndexBufferSRV;
		CERef<ConceptEngine::Graphics::Main::RenderLayer::CERayTracingGeometry> RayTracingGeometry;

		uint32 VertexCount = 0;
		uint32 IndexCount = 0;

		float ShadowOffset = 0.0f;

		Render::Scene::CEAABB BoundingBox;
	};
}
