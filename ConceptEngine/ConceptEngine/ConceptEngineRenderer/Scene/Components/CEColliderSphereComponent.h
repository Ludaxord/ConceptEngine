#pragma once
#include "Scene/Actor.h"


class CEColliderSphereComponent : public Component {
	CORE_OBJECT(CEColliderSphereComponent, Component);

public:
	CEColliderSphereComponent(Actor* InOwningActor): Component(InOwningActor), Material(nullptr), DebugMesh(nullptr) {
		CORE_OBJECT_INIT();

	}

	float Radius = 0.5f;
	DirectX::XMFLOAT3 Offset = {0.0f, 0.0f, 0.0f};
	bool IsTrigger = false;

	TSharedPtr<class CEPhysicsMaterial> Material;
	TSharedPtr<class Mesh> DebugMesh;
};
