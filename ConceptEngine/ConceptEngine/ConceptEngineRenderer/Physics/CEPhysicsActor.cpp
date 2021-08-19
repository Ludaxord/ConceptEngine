#include "CEPhysicsActor.h"


CEPhysicsActor::CEPhysicsActor(Actor* InActor): OwningActor(InActor),
                                                RigidBody(*InActor->GetComponentOfType<CERigidBodyComponent>()),
                                                LockFlags(0) {

}

CEPhysicsActor::~CEPhysicsActor() {
	Actor::~Actor();
	Colliders.Clear();
}

XMFLOAT3& CEPhysicsActor::GetPhysicsTranslation() const {
}

void CEPhysicsActor::SetPhysicsTranslation(const XMFLOAT3& Translation, bool AutoWake) {
	physx::PxTransform Transform = RigidActor->getGlobalPose();
	Transform.p = ToPhysXVector(Translation);
	RigidActor->setGlobalPose(Transform, AutoWake);
}

XMFLOAT3& CEPhysicsActor::GetPhysicsRotation() const {
}

void CEPhysicsActor::SetPhysicsRotation(const XMFLOAT3& Rotation, bool AutoWake) {
	physx::PxTransform Transform = RigidActor->getGlobalPose();
	Transform.q = ToPhysXQuat(XMFLOAT4(Rotation.x, Rotation.y, Rotation.z, 0.0f));
	RigidActor->setGlobalPose(Transform, AutoWake);
}

//TODO: Implement
void CEPhysicsActor::Rotate(const XMFLOAT3& Rotation, bool AutoWake) {
}

void CEPhysicsActor::WakeUp() {
	if (IsDynamic())
		RigidActor->is<physx::PxRigidDynamic>()->wakeUp();
}

void CEPhysicsActor::Hibernate() {
	if (IsDynamic())
		RigidActor->is<physx::PxRigidDynamic>()->putToSleep();
}

float CEPhysicsActor::GetMass() const {
	return !IsDynamic() ? RigidBody.Mass : RigidActor->is<physx::PxRigidDynamic>()->getMass();
}

//TODO: Implement...
void CEPhysicsActor::SetMass(float Mass) {
	if (IsDynamic()) {
		CE_LOG_ERROR("[CEPhysicsActor]: Cannot set mass of non-dynamic CEPhysicsActor.");
		return;
	}
}

//TODO: Implement...
void CEPhysicsActor::AddForce(const XMFLOAT3& Force, CEForceMode ForceMode) {
}

//TODO: Implement...
void CEPhysicsActor::AddTorque(const XMFLOAT3& Torque, CEForceMode ForceMode) {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetLinearVelocity() const {
}

//TODO: Implement...
void CEPhysicsActor::SetLinearVelocity(const XMFLOAT3& Velocity) {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetAngularVelocity() const {
}

//TODO: Implement...
void CEPhysicsActor::SetAngularVelocity(const XMFLOAT3& Velocity) {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetMaxLinearVelocity() const {
}

//TODO: Implement...
void CEPhysicsActor::SetMaxLinearVelocity(const XMFLOAT3& MaxVelocity) {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetMaxAngularVelocity() const {
}

//TODO: Implement...
void CEPhysicsActor::SetMaxAngularVelocity(const XMFLOAT3& MaxVelocity) {
}

//TODO: Implement...
void CEPhysicsActor::SetLinearDrag(float Drag) const {
}

//TODO: Implement...
void CEPhysicsActor::SetAngularDrag(float Drag) const {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetKinematicTargetPosition() const {
}

//TODO: Implement...
XMFLOAT3& CEPhysicsActor::GetKinematicTargetRotation() const {
}

//TODO: Implement...
void CEPhysicsActor::SetKinematicTarget(const XMFLOAT3& TargetPosition, const XMFLOAT3* TargetRotation) const {
}

//TODO: Implement...
void CEPhysicsActor::SetSimulation(uint32 Layer) {
}

//TODO: Implement...
bool CEPhysicsActor::IsDynamic() const {
}

//TODO: Implement...
bool CEPhysicsActor::IsKinematic() const {
}

//TODO: Implement...
void CEPhysicsActor::SetKinematic(bool IsKinematic) {
}

//TODO: Implement...
bool CEPhysicsActor::IsGravityDisabled() const {
}

//TODO: Implement...
void CEPhysicsActor::SetGravityDisabled(bool Disable) {
}

//TODO: Implement...
bool CEPhysicsActor::GetLockFlag(CEActorLockFlag Flag) const {
}

//TODO: Implement...
void CEPhysicsActor::SetLockFlag(CEActorLockFlag Flag, bool FlagValue) {
}

//TODO: Implement...
void CEPhysicsActor::OnFixedUpdate(CETimestamp FixedDeltaTime) {
}

//TODO: Implement...
Actor* CEPhysicsActor::GetOwningActor() const {
}

//TODO: Implement...
const CETransformComponent& CEPhysicsActor::GetTransform() const {
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderBoxComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderSphereComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderCapsuleComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderMeshComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
}
