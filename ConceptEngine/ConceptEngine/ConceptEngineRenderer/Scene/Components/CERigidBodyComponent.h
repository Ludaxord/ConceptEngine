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

	Type BodyType = Type::Static;
	float Mass = 1.0f;
	float LinearDrag = 0.0f;
	float AngularDrag = 0.05f;
	bool DisableGravity = false;
	bool IsKinematic = false;
	uint32 Layer = 0;
	CollisionDetectionType CollisionDetection = CollisionDetectionType::Discrete;

	bool LockPositionX = false;
	bool LockPositionY = false;
	bool LockPositionZ = false;
	bool LockRotationX = false;
	bool LockRotationY = false;
	bool LockRotationZ = false;
};
