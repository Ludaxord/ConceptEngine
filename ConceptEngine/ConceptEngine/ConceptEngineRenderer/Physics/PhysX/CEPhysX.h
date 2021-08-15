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

#include "CECookingFactory.h"
#include "CEPhysXDebugger.h"
#include "CEPhysXManager.h"

class CEPhysXErrorCallback : public physx::PxErrorCallback {
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

class CEPhysXAssertHandler : public physx::PxAssertHandler {
public:
	void operator()(const char* exp, const char* file, int line, bool& ignore) override;
};

class CEPhysX : public CEPhysics {
public:
	CEPhysX();
	~CEPhysX();

	bool Create() override;
	void Release() override;
	bool CreateScene() override;
	void ReleaseScene() override;
	bool CreateConfig() override;
	void CreateActors(Scene* Scene) override;
	CEPhysicsActor* CreateActor(Actor* InActor) override;

	bool CreateDebugger();

private:
	bool CreatePhysXInternals();

private:
	CEPhysXManager* PhysicsManager;
	CEPhysXDebugger* PhysicsDebugger;
	CECookingFactory* CookingFactory;
};
