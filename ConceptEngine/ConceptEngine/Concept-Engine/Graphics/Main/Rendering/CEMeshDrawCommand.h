#pragma once

namespace ConceptEngine::Graphics::Main::Rendering {
	struct CEMeshDrawCommand {
		class CEMaterial* Material = nullptr;
		class CEMesh* Mesh = nullptr;
		class CEActor* CurrentActor = nullptr;
		
		class CEVertexBuffer* VertexBuffer = nullptr;
		class CEIndexBuffer* IndexBuffer = nullptr;

		class CERayTracingGeometry* Geometry = nullptr;
	};
}