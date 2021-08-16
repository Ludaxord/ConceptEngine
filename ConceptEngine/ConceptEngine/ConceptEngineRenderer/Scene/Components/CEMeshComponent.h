#pragma once
#include "../../Scene/Actor.h"

class CEMeshComponent : public Component {
	CORE_OBJECT(CEMeshComponent, Component);

public:
	CEMeshComponent(Actor* InOwningActor)
		: Component(InOwningActor)
		  , Material(nullptr)
		  , Mesh(nullptr) {
		CORE_OBJECT_INIT();
	}

	TSharedPtr<class Material> Material;
	TSharedPtr<class Mesh> Mesh;
};
