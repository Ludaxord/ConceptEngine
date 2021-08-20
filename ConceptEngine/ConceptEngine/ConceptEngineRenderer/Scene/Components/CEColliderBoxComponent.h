#pragma once
#include "Scene/Actor.h"

class CEColliderBoxComponent : public Component {
	CORE_OBJECT(CEColliderBoxComponent, Component);

public:
	CEColliderBoxComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}
};
