#include "CEPhysX.h"

CEPhysX::CEPhysX() {
}

CEPhysX::~CEPhysX() {
}

bool CEPhysX::Create() {
	return false;
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
