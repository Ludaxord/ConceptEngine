#pragma once
#include "../Scene/Actor.h"

class CEIDComponent : public Component {
	CORE_OBJECT(CEIDComponent, Component);

public:
	CEIDComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	CEUUID ID = 0;
};
