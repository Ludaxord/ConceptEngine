#include "CEPhysicsActor.h"

#include "CEPhysicsManagers.h"
#include "PhysX/CEPhysXManager.h"


CEPhysicsActor::CEPhysicsActor(Actor* InActor): OwningActor(InActor),
                                                RigidBody(InActor->GetComponentOfType<CERigidBodyComponent>()),
                                                LockFlags(0) {

}

CEPhysicsActor::~CEPhysicsActor() {
	Actor::~Actor();
	Colliders.Clear();
}

XMFLOAT3 CEPhysicsActor::GetPhysicsTranslation() const {
	return FromPhysXVector(RigidActor->getGlobalPose().p);
}

void CEPhysicsActor::SetPhysicsTranslation(const XMFLOAT3& Translation, bool AutoWake) {
	physx::PxTransform PhysicsTransform = RigidActor->getGlobalPose();
	PhysicsTransform.p = ToPhysXVector(Translation);
	RigidActor->setGlobalPose(PhysicsTransform, AutoWake);
}

XMFLOAT3 CEPhysicsActor::GetPhysicsRotation() const {
	auto Quat = FromPhysXQuat(RigidActor->getGlobalPose().q);
	DirectX::XMFLOAT3 EulerFloat3;
	XMStoreFloat3(&EulerFloat3, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4(&Quat)));
	return EulerFloat3;
}

void CEPhysicsActor::SetPhysicsRotation(const XMFLOAT3& Rotation, bool AutoWake) {
	physx::PxTransform PhysicsTransform = RigidActor->getGlobalPose();
	PhysicsTransform.q = ToPhysXQuat(XMFLOAT4(Rotation.x, Rotation.y, Rotation.z, 0.0f));
	RigidActor->setGlobalPose(PhysicsTransform, AutoWake);
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
	return !IsDynamic() ? RigidBody->Mass : RigidActor->is<physx::PxRigidDynamic>()->getMass();
}

void CEPhysicsActor::SetMass(float Mass) {
	if (IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set mass of non-dynamic CEPhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	physx::PxRigidBodyExt::setMassAndUpdateInertia(*RigidDynamic, Mass);
	RigidBody->Mass = Mass;
}

void CEPhysicsActor::AddForce(const XMFLOAT3& Force, CEForceMode ForceMode) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot add Force to non-dynamic PhysicsActor.");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->addForce(ToPhysXVector(Force), (physx::PxForceMode::Enum)ForceMode);
}

void CEPhysicsActor::AddTorque(const XMFLOAT3& Torque, CEForceMode ForceMode) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot add Torque to non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->addTorque(ToPhysXVector(Torque), (physx::PxForceMode::Enum)ForceMode);
}

//TODO: Implement...
XMFLOAT3 CEPhysicsActor::GetLinearVelocity() const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get velocity of non-dynamic PhysicsActor");
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	return FromPhysXVector(RigidDynamic->getLinearVelocity());
}

void CEPhysicsActor::SetLinearVelocity(const XMFLOAT3& Velocity) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setLinearVelocity(ToPhysXVector(Velocity));
}

XMFLOAT3 CEPhysicsActor::GetAngularVelocity() const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get velocity of non-dynamic PhysicsActor");
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	return FromPhysXVector(RigidDynamic->getAngularVelocity());
}

void CEPhysicsActor::SetAngularVelocity(const XMFLOAT3& Velocity) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setAngularVelocity(ToPhysXVector(Velocity));
}

float CEPhysicsActor::GetMaxLinearVelocity() const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get velocity of non-dynamic PhysicsActor");
		return 0.0f;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	return RigidDynamic->getMaxLinearVelocity();
}

void CEPhysicsActor::SetMaxLinearVelocity(float MaxVelocity) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setMaxLinearVelocity(MaxVelocity);
}

float CEPhysicsActor::GetMaxAngularVelocity() const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get velocity of non-dynamic PhysicsActor");
		return 0.0f;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	return RigidDynamic->getMaxAngularVelocity();
}

void CEPhysicsActor::SetMaxAngularVelocity(float MaxVelocity) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setMaxAngularVelocity(MaxVelocity);
}

void CEPhysicsActor::SetLinearDrag(float Drag) const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setLinearDamping(Drag);
}

void CEPhysicsActor::SetAngularDrag(float Drag) const {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set velocity of non-dynamic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setAngularDamping(Drag);
}

XMFLOAT3 CEPhysicsActor::GetKinematicTargetPosition() const {
	if (!IsKinematic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get kinematic target position for non-kinematic PhysicsActor");
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	physx::PxTransform Target;
	RigidDynamic->getKinematicTarget(Target);
	return FromPhysXVector(Target.p);
}

XMFLOAT3 CEPhysicsActor::GetKinematicTargetRotation() const {
	if (!IsKinematic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot get kinematic target rotation for non-kinematic PhysicsActor");
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	physx::PxTransform Target;
	RigidDynamic->getKinematicTarget(Target);
	auto Quat = FromPhysXQuat(Target.q);
	DirectX::XMFLOAT3 EulerFloat3;
	XMStoreFloat3(&EulerFloat3, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4(&Quat)));
	return EulerFloat3;
}

void CEPhysicsActor::SetKinematicTarget(const XMFLOAT3& TargetPosition, const XMFLOAT3& TargetRotation) const {
	if (!IsKinematic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set kinematic target for a non-kinematic PhysicsActor");
		return;
	}

	physx::PxRigidDynamic* RigidDynamic = RigidActor->is<physx::PxRigidDynamic>();
	Assert(RigidDynamic);
	RigidDynamic->setKinematicTarget(ToPhysXTransform(TargetPosition, TargetRotation));
}

void CEPhysicsActor::SetSimulation(uint32 Layer) {
	const CEPhysicsLayer& LayerInfo = PhysicsManager->GetLayer(Layer);
	if (LayerInfo.CollidesWith == 0)
		return;

	physx::PxFilterData FilterData;
	FilterData.word0 = LayerInfo.BitValue;
	FilterData.word1 = LayerInfo.CollidesWith;
	FilterData.word2 = (uint32)RigidBody->CollisionDetection;

	for (auto& Collider : Colliders)
		Collider->SetFilter(FilterData);
}

bool CEPhysicsActor::IsDynamic() const {
	return RigidBody->BodyType == CERigidBodyComponent::Type::Dynamic;
}

bool CEPhysicsActor::IsKinematic() const {
	return IsDynamic() && RigidBody->IsKinematic;
}

void CEPhysicsActor::SetKinematic(bool IsKinematic) {
	if (!IsDynamic()) {
		CE_LOG_WARNING("[CEPhysicsActor]: Cannot set kinematic for a static PhysicsActor ")
		return;
	}

	RigidBody->IsKinematic = IsKinematic;
	RigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, IsKinematic);
}

//TODO: Implement...
bool CEPhysicsActor::IsGravityDisabled() const {
	return RigidActor->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
}

void CEPhysicsActor::SetGravityDisabled(bool Disable) {
	RigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, Disable);
}

//TODO: Implement...
bool CEPhysicsActor::GetLockFlag(CEActorLockFlag Flag) const {
	return false;
}

void CEPhysicsActor::SetLockFlag(CEActorLockFlag Flag, bool FlagValue) {
	if (FlagValue)
		LockFlags |= (uint32)Flag;
	else
		LockFlags &= ~(uint32)Flag;

	if (IsDynamic())
		return;

	RigidActor->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlag(ToPhysXActorLockFlag(Flag), FlagValue);
}

//TODO: Implement...
void CEPhysicsActor::OnFixedUpdate(CETimestamp FixedDeltaTime) {
}

//TODO: Implement...
Actor* CEPhysicsActor::GetOwningActor() const {
	return nullptr;
}

//TODO: Implement...
const CETransformComponent& CEPhysicsActor::GetTransform() const {
	return CETransformComponent(nullptr);
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderBoxComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
	Colliders.PushBack(CEBoxColliderShape::Instance());
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderSphereComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
	Colliders.PushBack(CESphereColliderShape::Instance());
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderCapsuleComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
	Colliders.PushBack(CECapsuleColliderShape::Instance());
}

//TODO: Implement...
void CEPhysicsActor::AddCollider(CEColliderMeshComponent& Collider, Actor* Actor, const XMFLOAT3& Offset) {
	if (Collider.IsConvex) {
		Colliders.PushBack(CECapsuleColliderShape::Instance());
	}
	else {
		if (IsDynamic() && !IsKinematic()) {
			CE_LOG_ERROR(
				"[CEPhysicsActor]: Cannot have a non-convex MeshColliderComponent for a non-kinematic dynamic RigidBodyComponent!")
		}

		Colliders.PushBack(CETriangleMeshShape::Instance());
	}
}
