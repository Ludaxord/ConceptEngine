#pragma once
#include "Physics/CEPhysicsActor.h"

#include <PxPhysics.h>
#include "Physics/CEPhysicsMaterial.h"
#include "Rendering/Resources/Mesh.h"

class CEPhysXActor : public CEPhysicsActor {
	CORE_OBJECT(CEPhysXActor, CEPhysicsActor);
public:
	CEPhysXActor(Actor* InActor);
	~CEPhysXActor();

	physx::PxRigidActor& GetPhysXActor() const {
		return *RigidActor;
	}

private:
	physx::PxRigidActor* RigidActor;

	friend class CEPhysicsScene;
};
