#pragma once

#include "../Managers/CEMeshManager.h"

#include "../../../Render/Scene/CEAABB.h"

#include "../RenderLayer/CECommandList.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CEResourceViews.h"
#include "../RenderLayer/CERayTracing.h"
#include "../../../Core/Containers/CESharedPtr.h"


namespace ConceptEngine::Graphics::Main::Common {
	class CEMesh {
	public:
		CEMesh() = default;
		~CEMesh() = default;

		bool Create(const CEMeshData& data);

		bool BuildAccelerationStructure(ConceptEngine::Graphics::Main::RenderLayer::CECommandList& commandList);

		static CESharedPtr<CEMesh> Make(const CEMeshData& data);

		void CreateBoundingBox(const CEMeshData& data);

		CERef<RenderLayer::CEVertexBuffer> VertexBuffer;
		CERef<RenderLayer::CEShaderResourceView> VertexBufferSRV;
		CERef<RenderLayer::CEIndexBuffer> IndexBuffer;
		CERef<RenderLayer::CEShaderResourceView> IndexBufferSRV;
		CERef<RenderLayer::CERayTracingGeometry> RTGeometry;

		uint32 VertexCount = 0;
		uint32 IndexCount = 0;

		float ShadowOffset = 0.0f;

		Render::Scene::CEAABB BoundingBox;
	};
}
