#include "CEPhysX.h"

#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <PxScene.h>
#include <characterkinematic/PxCapsuleController.h>
#include <characterkinematic/PxControllerManager.h>
#include <common/PxTolerancesScale.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxVec4.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>

struct CEPhysXInternals {
	physx::PxFoundation* PhysXFoundation;
	physx::PxDefaultCpuDispatcher* PhysXCPUDispatcher;
	physx::PxPhysics* PhysXSDK;
	physx::PxDefaultAllocator* Allocator;
	CEPhysXErrorCallback* ErrorCallback;
	CEPhysXAssertHandler* AssertHandler;
};

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	PhysicsManager = new CEPhysXManager();


	return true;
}

void CEPhysX::Release() {
}

void CEPhysX::ReleaseScene() {
}

void CEPhysX::CreateActors(Scene* Scene) {
}

CEPhysicsActor* CEPhysX::CreateActor(Actor* InActor) {
	return {};
}

bool CEPhysX::CreateScene() {

	return true;
}


bool CEPhysX::CreatePhysXInternals() {

}
