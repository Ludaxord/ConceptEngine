#pragma once

#include "../Common/CEMesh.h"
#include "../../../Render/Scene/CEActor.h"

namespace ConceptEngine::Graphics::Main { namespace Common {
		class CEMaterial;
	}

	namespace Rendering {
		struct CEMeshDrawCommand {
			class Common::CEMaterial* Material = nullptr;
			class Common::CEMesh* Mesh = nullptr;
			class ConceptEngine::Render::Scene::CEActor* CurrentActor = nullptr;

			class CEVertexBuffer* VertexBuffer = nullptr;
			class CEIndexBuffer* IndexBuffer = nullptr;

			class CERayTracingGeometry* Geometry = nullptr;
		};
	}}
