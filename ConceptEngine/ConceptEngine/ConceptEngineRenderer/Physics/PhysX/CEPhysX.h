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
#include <PxFiltering.h>
#include <PxBroadPhase.h>
#include <PxSceneDesc.h>

#include "CECookingFactory.h"
#include "CEPhysXDebugger.h"
#include "CEPhysXManager.h"
#include "Config/CEGlobalConfigs.h"

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
	void CreateActors(Scene* Scene) override;
	CEPhysicsActor* CreateActor(Actor* InActor) override;

	bool CreateConfig() override;
	bool CreateDebugger();
	bool CreateCookingFactory();

	static physx::PxFoundation& GetFoundation() ;
	static physx::PxPhysics& GetPhysXSDK();
	static physx::PxCpuDispatcher& GetCPUDispatcher();
	static physx::PxDefaultAllocator& GetAllocator();

	static physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes Attr0, physx::PxFilterData FilterData0,
	                                         physx::PxFilterObjectAttributes Attr1, physx::PxFilterData FilterData1,
	                                         physx::PxPairFlags& PairFlags,
	                                         const void* ConstantBlock, physx::PxU32 ConstantBlockSize);

	static physx::PxBroadPhaseType::Enum GetPhysXBroadPhaseType(PhysicsBroadPhaseType Type);
	static physx::PxFrictionType::Enum GetPhysXFrictionType(PhysicsFrictionType Type);

private:
	bool CreatePhysXInternals();

private:
	CEPhysXManager* PhysicsManager;
	CEPhysXDebugger* PhysicsDebugger;
	CECookingFactory* CookingFactory;
};
