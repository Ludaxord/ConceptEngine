#pragma once
#include "PhysX/CEPhysXShapes.h"
#include "PhysX/CePhysXUtils.h"
#include "Scene/Actor.h"
#include "Scene/Components/CEColliderBoxComponent.h"
#include "Scene/Components/CEColliderCapsuleComponent.h"
#include "Scene/Components/CEColliderMeshComponent.h"
#include "Scene/Components/CEColliderSphereComponent.h"
#include "Scene/Components/CERigidBodyComponent.h"
#include "Scene/Components/CETransformComponent.h"

#include <PxPhysicsAPI.h>

//TODO:  check which way is more suitable, Actor or CoreObject...
class CEPhysicsActor : public Actor {
	CORE_OBJECT(CEPhysicsActor, Actor);

public:
	CEPhysicsActor(Actor* InActor);
	~CEPhysicsActor();

	//TODO: Create container for all transformations of PhysicsActor.
	//TODO: Move all PhysX specific functions to PhysXActor
	XMFLOAT3& GetPhysicsTranslation() const;
	void SetPhysicsTranslation(const XMFLOAT3& Translation, bool AutoWake = true);

	XMFLOAT3& GetPhysicsRotation() const;
	void SetPhysicsRotation(const XMFLOAT3& Rotation, bool AutoWake = true);

	void Rotate(const XMFLOAT3& Rotation, bool AutoWake = true);

	void WakeUp();
	void Hibernate();

	float GetMass() const;
	void SetMass(float Mass);

	void AddForce(const XMFLOAT3& Force, CEForceMode ForceMode);
	void AddTorque(const XMFLOAT3& Torque, CEForceMode ForceMode);

	XMFLOAT3 GetLinearVelocity() const;
	void SetLinearVelocity(const XMFLOAT3& Velocity);
	XMFLOAT3 GetAngularVelocity() const;
	void SetAngularVelocity(const XMFLOAT3& Velocity);

	float GetMaxLinearVelocity() const;
	void SetMaxLinearVelocity(float MaxVelocity);
	float GetMaxAngularVelocity() const;
	void SetMaxAngularVelocity(float MaxVelocity);

	void SetLinearDrag(float Drag) const;
	void SetAngularDrag(float Drag) const;

	XMFLOAT3 GetKinematicTargetPosition() const;
	XMFLOAT3 GetKinematicTargetRotation() const;
	void SetKinematicTarget(const XMFLOAT3& TargetPosition, const XMFLOAT3& TargetRotation) const;

	void SetSimulation(uint32 Layer);

	bool IsDynamic() const;

	bool IsKinematic() const;
	void SetKinematic(bool IsKinematic);

	bool IsGravityDisabled() const;
	void SetGravityDisabled(bool Disable);

	bool GetLockFlag(CEActorLockFlag Flag) const;
	void SetLockFlag(CEActorLockFlag Flag, bool FlagValue);

	void OnFixedUpdate(CETimestamp FixedDeltaTime);

	Actor* GetOwningActor() const;

	const CETransformComponent& GetTransform() const;

	void AddCollider(CEColliderBoxComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderSphereComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderCapsuleComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderMeshComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);

protected:
	Actor* OwningActor;
	CERigidBodyComponent RigidBody;
	uint32 LockFlags;

	CEArray<CEColliderShape*> Colliders;

	//TODO: Move to PhysXActor
	physx::PxRigidActor* RigidActor;

	friend class CEPhysicsScene;
};
