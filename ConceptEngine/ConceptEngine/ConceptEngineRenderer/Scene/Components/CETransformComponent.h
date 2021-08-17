#pragma once
#include "Scene/Actor.h"

class CETransformComponent : public Component {
	CORE_OBJECT(CETransformComponent, Component);

public:
	CETransformComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}
};
