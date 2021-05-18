#pragma once


namespace ConceptEngine::Graphics::Main { namespace Common {
		class CEMaterial;
	}

	namespace Rendering {
		struct CEMeshDrawCommand {
			class Common::CEMaterial* Material = nullptr;
			class CEMesh* Mesh = nullptr;
			class CEActor* CurrentActor = nullptr;

			class CEVertexBuffer* VertexBuffer = nullptr;
			class CEIndexBuffer* IndexBuffer = nullptr;

			class CERayTracingGeometry* Geometry = nullptr;
		};
	}}
