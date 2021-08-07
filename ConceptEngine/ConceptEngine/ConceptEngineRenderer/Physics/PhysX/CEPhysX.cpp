#include "CEPhysX.h"

#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxPhysicsVersion.h>
#include <PxScene.h>
#include <characterkinematic/PxControllerManager.h>
#include <common/PxTolerancesScale.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <pvd/PxPvd.h>
#include <pvd/PxPvdTransport.h>

#define MAX_NUM_ACTOR_SHAPES 128

extern physx::PxDefaultAllocator PhysXAllocator;
extern physx::PxDefaultErrorCallback PhysXErrorCallback;

extern physx::PxFoundation* PhysXFoundation;
extern physx::PxPhysics* PhysXPhysics;

extern physx::PxDefaultCpuDispatcher* PhysXDispatcher;
extern physx::PxScene* PhysXScene;

extern physx::PxControllerManager* PhysXControllerManager;

extern physx::PxPvd* PhysXPvd;

std::unordered_map<std::string, physx::PxRigidDynamic*> PxRigidDynamicMap;
std::unordered_map<std::string, physx::PxRigidStatic*> PxRigidStaticMap;
std::unordered_map<std::string, physx::PxController*> PxControllerMap;

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	PhysXFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, PhysXAllocator, PhysXErrorCallback);

	if (!PhysXFoundation) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Foundation");
		return false;
	}

	PhysXPvd = physx::PxCreatePvd(*PhysXFoundation);
	if (!PhysXPvd) {
		CE_LOG_ERROR("[CEPhysX]: Failed to Create PhysX Foundation");
	}
	else {
		physx::PxPvdTransport* Transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		if (!PhysXPvd->connect(*Transport, physx::PxPvdInstrumentationFlag::eALL)) {
			CE_LOG_WARNING("[CEPhysX]: Failed to Connect to PhysX Pvd")
		}
	}


	PhysXPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysXFoundation, physx::PxTolerancesScale(), true, PhysXPvd);
	if (!PhysXPhysics) {
		CE_LOG_ERROR("[CEPhysX]: Failed to create PhysX Physics")
		return false;
	}

	return true;
}

bool CEPhysX::CreateScene() {
	return false;
}

void CEPhysX::Update(CETimestamp DeltaTime) {
}

std::string CEPhysX::CreatePXRigidStatic(void* Desc) {
	return {};
}

std::string CEPhysX::CreatePXRigidDynamic(void* Desc) {
	return {};
}

void CEPhysX::AddForce(std::string Name, physx::PxVec3 Force) {
}

void CEPhysX::SetRigidDynamicLockFlag(std::string Name, int Axis, bool DynamicLockFlag) {
}

void CEPhysX::SetAngularDamping(std::string Name, float AngularDamping) {
}

void CEPhysX::SetLinearVelocity(std::string Name, physx::PxVec3 LinearVelocity) {
}

void CEPhysX::SetKinematicFlag(std::string Name, bool KinematicFlag) {
}

void CEPhysX::SetKinematicTarget(std::string Name, physx::PxVec3& Position, physx::PxVec4& Quat) {
}

void CEPhysX::ReleasePXRigidStatic(std::string Name) {
}

void CEPhysX::ReleasePXRigidDynamic(std::string Name) {
}

std::string CEPhysX::AddCharacterController(void* Desc) {
	return {};
}

void CEPhysX::ReleaseCharacterController(const std::string& Name) {
}

int32 CEPhysX::MoveCharacterController(const std::string& Name, const physx::PxVec3& Disp, float MinDist,
                                       float ElapsedTime) {
	return 0;
}

physx::PxVec3 CEPhysX::GetCharacterControllerTranslation(const std::string& Name) {
	return {};
}

void CEPhysX::GetPXRigidDynamicTransform(std::string Name, physx::PxVec3& Position, physx::PxVec4& Quat) {
}

void CEPhysX::Release() {
	PxRigidDynamicMap.clear();
	PxRigidStaticMap.clear();
	SafePhysXRelease(PhysXScene);
}

bool CEPhysX::HasPXRigidStatic(std::string& Name) {
	return false;
}

bool CEPhysX::HasPXRigidDynamic(std::string& Name) {
	return false;
}

bool CEPhysX::HasPXController(std::string& Name) {
	return false;
}
