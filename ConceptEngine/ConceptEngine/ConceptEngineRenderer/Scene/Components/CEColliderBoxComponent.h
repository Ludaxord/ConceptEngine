#pragma once
#include "Scene/Actor.h"

class CEColliderBoxComponent : public Component {
	CORE_OBJECT(CEColliderBoxComponent, Component);

public:
	CEColliderBoxComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	DirectX::XMFLOAT2 Size = {};
	DirectX::XMFLOAT2 Offset = {};

	bool IsTrigger = false;

	TSharedPtr<class CEPhysicsMaterial> Material;
	TSharedPtr<class Mesh> DebugMesh;

};
