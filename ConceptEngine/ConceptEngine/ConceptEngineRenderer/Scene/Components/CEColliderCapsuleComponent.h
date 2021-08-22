#pragma once
#include "Scene/Actor.h"

class CEColliderCapsuleComponent : public Component {
	CORE_OBJECT(CEColliderCapsuleComponent, Component);

public:
	CEColliderCapsuleComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	float Radius = 0.5f;
	float Height = 1.0f;
	XMFLOAT3 Offset = {0.0f, 0.0f, 0.0f};
	bool IsTrigger = false;

	TSharedPtr<class CEPhysicsMaterial> Material;
	TSharedPtr<class Mesh> DebugMesh;
};
