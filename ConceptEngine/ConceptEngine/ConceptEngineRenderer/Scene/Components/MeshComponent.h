#pragma once
#include "../../Scene/Actor.h"

class MeshComponent : public Component {
	CORE_OBJECT(MeshComponent, Component);

public:
	MeshComponent(Actor* InOwningActor)
		: Component(InOwningActor)
		  , Material(nullptr)
		  , Mesh(nullptr)
	      , RigidStatic(nullptr)
		  , RigidDynamic(nullptr) {
		CORE_OBJECT_INIT();
	}

	virtual void Update(CETimestamp DeltaTime) override {
		auto RigidTransform = OwningActor->GetTransform().AsRigidTransform();
		if (RigidDynamic) {
			RigidDynamic->Update(RigidTransform);
		}
	}

	TSharedPtr<class Material> Material;
	TSharedPtr<class Mesh> Mesh;
	TSharedPtr<class CERigidStatic> RigidStatic;
	TSharedPtr<class CERigidDynamic> RigidDynamic;
};
