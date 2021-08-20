#pragma once
#include "../Scene/Actor.h"


class CELinkComponent : public Component {
	CORE_OBJECT(CELinkComponent, Component);

public:
	CELinkComponent(Actor* InOwningActor): Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}

	CEUUID ParentUUID = 0;
	CEArray<CEUUID> ChildrenUUIDs{};
};
