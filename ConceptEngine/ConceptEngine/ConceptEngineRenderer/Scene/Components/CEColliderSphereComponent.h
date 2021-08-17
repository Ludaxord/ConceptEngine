#pragma once
#include "Scene/Actor.h"


class CEColliderSphereComponent : public Component {
	CORE_OBJECT(CEColliderSphereComponent, Component);

public:
	CEColliderSphereComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();

	}
};
