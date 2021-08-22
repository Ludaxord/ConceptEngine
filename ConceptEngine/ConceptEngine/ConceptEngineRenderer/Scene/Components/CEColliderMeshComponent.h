#pragma once
#include "Scene/Actor.h"

class CEColliderMeshComponent : public Component {
	CORE_OBJECT(CEColliderMeshComponent, Component);

public:
	CEColliderMeshComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	bool IsConvex = false;
	bool IsTrigger = false;
	bool OverrideMesh = false;

	TSharedPtr<class Mesh> CollisionMesh;
	TSharedPtr<class CEPhysicsMaterial> Material;

	operator TSharedPtr<Mesh>() {
		return CollisionMesh;
	}
};
