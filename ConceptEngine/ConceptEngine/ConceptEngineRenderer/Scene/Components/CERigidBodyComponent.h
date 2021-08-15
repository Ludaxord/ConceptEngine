#pragma once
#include "Scene/Actor.h"

class CERigidBodyComponent : public Component {
	CORE_OBJECT(CERigidBodyComponent, Component);

public:
	CERigidBodyComponent(Actor* InOwningActor)
		: Component(InOwningActor) {
		CORE_OBJECT_INIT();
	}


	enum class Type { Static, Dynamic };

	enum class CollisionDetectionType { Discrete, Continuous, ContinuousSpeculative };

	virtual void Update(CETimestamp DeltaTime) override {
	}

	CollisionDetectionType CollisionDetection = CollisionDetectionType::Discrete;
};
