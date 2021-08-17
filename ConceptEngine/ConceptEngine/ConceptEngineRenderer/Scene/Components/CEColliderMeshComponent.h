#pragma once
#include "Scene/Actor.h"

class CEColliderMeshComponent : public Component {
	CORE_OBJECT(CEColliderMeshComponent, Component);

public:
	CEColliderMeshComponent(Actor* InOwningActor): Component(InOwningActor) {
				CORE_OBJECT_INIT();
	}
};
