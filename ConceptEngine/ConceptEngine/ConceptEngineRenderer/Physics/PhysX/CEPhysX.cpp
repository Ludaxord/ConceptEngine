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

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	return true;
}

void CEPhysX::Release() {
}

void CEPhysX::ReleaseScene() {
}

void CEPhysX::CreateActors(Scene* Scene) {
}

CERef<CEPhysicsActor> CEPhysX::CreateActor(Actor* InActor) {
	return {};
}

bool CEPhysX::CreateScene() {
	
	return true;
}
