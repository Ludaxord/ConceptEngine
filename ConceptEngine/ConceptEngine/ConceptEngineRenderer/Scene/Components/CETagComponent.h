#pragma once
#include "Scene/Actor.h"

class CETagComponent : public Component {
	CORE_OBJECT(CETagComponent, Component);

public:
	CETagComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	std::string Tag;
};
