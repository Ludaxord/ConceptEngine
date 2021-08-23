#pragma once
#include "CEPhysicsActor.h"
#include "Scene/Actor.h"

#define OVERLAP_MAX_COLLISION 10

struct CERayCastHit {
	uint64 HitEntity;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	float Distance;
};

class CEPhysicsScene {
public:
	CEPhysicsScene() {
	};
	virtual ~CEPhysicsScene() = default;

	virtual void Release();

	void Simulate(float TS, bool CallFixedUpdate = true);

	CEPhysicsActor* GetActor(Actor* Actor);
	CEPhysicsActor* CreateActor(Actor* Actor);
	void RemoveActor(CEPhysicsActor* PhysicsActor);

	DirectX::XMFLOAT3 GetGravity() const {
		return FromPhysXVector(PhysXScene->getGravity());
	}

	void SetGravity(const XMFLOAT3& Gravity) {
		PhysXScene->setGravity(ToPhysXVector(Gravity));
	}

	bool Raycast(const XMFLOAT3& Origin, const XMFLOAT3& Direction, float MaxDistance, CERayCastHit* OutHit);
	bool OverlapBox(const XMFLOAT3& Origin,
	                const XMFLOAT3& HalfSize,
	                CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
	                uint32& Count);
	bool OverlapCapsule(const XMFLOAT3& Origin,
	                    float Radius,
	                    float HalfHeight,
	                    CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
	                    uint32& Count);
	bool OverlapSphere(const XMFLOAT3& Origin,
	                   float Radius,
	                   CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
	                   uint32& Count);

	bool IsValid() const {
		return PhysXScene != nullptr;
	}

private:
	friend class CEPhysics;
	friend class CEPhysX;

	void CreateRegions();

	bool Advance(CETimestamp TS);
	void SubStepStrategy(CETimestamp TS);

	bool OverlapGeometry(DirectX::XMFLOAT3& Origin,
	                     const physx::PxGeometry& Geometry,
	                     CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
	                     uint32& Count);

	physx::PxScene* PhysXScene;

	CEArray<CEPhysicsActor*> Actors;

	float SubStepSize;
	float Accumulator = 0.0f;
	uint32 NumSubSteps = 0;
	const uint32 MaxSubSteps = 8;

};
