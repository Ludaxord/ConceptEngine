#pragma once
#include <characterkinematic/PxControllerManager.h>
#include <characterkinematic/PxExtended.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>

#include "../Physics/CEPhysics.h"
#include <foundation/PxVec3.h>
#include <foundation/PxVec4.h>
#include <pvd/PxPvd.h>

class CEPhysX : public CEPhysics {
public:
	CEPhysX();
	~CEPhysX();

	bool Create() override;
	void Release() override;
	bool CreateScene() override;
	void ReleaseScene() override;
	void CreateActors(Scene* Scene) override;
	CERef<CEPhysicsActor> CreateActor(Actor* InActor) override;
};

