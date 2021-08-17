#pragma once
#include "CEPhysicsActor.h"
#include "Scene/Actor.h"

class CEPhysicsScene {
public:
	CEPhysicsScene() {
	};
	virtual ~CEPhysicsScene() = default;

	virtual void Release() = 0;
	CEPhysicsActor* GetActor(Actor* Actor);
	CEPhysicsActor* CreateActor(Actor* actor);

private:
	CEArray<CEPhysicsActor*> Actors;
};
