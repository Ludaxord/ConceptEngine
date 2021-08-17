#pragma once
#include "Scene/Actor.h"

class CEColliderCapsuleComponent : public Component {
	CORE_OBJECT(CEColliderCapsuleComponent, Component);

public:
	CEColliderCapsuleComponent(Actor* InOwningActor): Component(InOwningActor) {
				CORE_OBJECT_INIT();
	}
};