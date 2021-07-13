#pragma once

#include "../CEActor.h"
#include "../../Core/Containers/CESharedPtr.h"

namespace ConceptEngine::Render::Scene::Components {
	class CEMeshComponent : public CEComponent {
		CORE_OBJECT(CEMeshComponent, CEComponent);

	public:
		CEMeshComponent(CEActor* OwningActor) : CEComponent(OwningActor), Material(nullptr), Mesh(nullptr) {
			CORE_OBJECT_INIT();
		}

		CESharedPtr<class Graphics::Main::Common::CEMaterial> Material; 
		CESharedPtr<class Graphics::Main::Common::CEMesh> Mesh; 
	};
}
