#pragma once
#include "Scene/Actor.h"

class CEColliderBoxComponent : public Component {
	CORE_OBJECT(CEColliderBoxComponent, Component);

public:
	CEColliderBoxComponent(Actor* InOwningActor): Component(InOwningActor), Material(nullptr), DebugMesh(nullptr) {
		CORE_OBJECT_INIT();
	}

	DirectX::XMFLOAT3 Size = {1.0f, 1.0f, 1.0f};
	DirectX::XMFLOAT3 Offset = {0.0f, 0.0f, 0.0f};

	bool IsTrigger = false;

	TSharedPtr<class CEPhysicsMaterial> Material;
	TSharedPtr<class Mesh> DebugMesh;

};
