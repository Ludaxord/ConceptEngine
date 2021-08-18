#pragma once
#include "Scene/Actor.h"
#include "Scene/Components/CEColliderBoxComponent.h"
#include "Scene/Components/CEColliderCapsuleComponent.h"
#include "Scene/Components/CEColliderMeshComponent.h"
#include "Scene/Components/CEColliderSphereComponent.h"

class CEPhysicsActor : public Actor {
public:
	void AddCollider(CEColliderBoxComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderSphereComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderCapsuleComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void AddCollider(CEColliderMeshComponent& Collider, Actor* Actor, const XMFLOAT3& Offset);
	void SetSimulation(uint32 Layer);
};
