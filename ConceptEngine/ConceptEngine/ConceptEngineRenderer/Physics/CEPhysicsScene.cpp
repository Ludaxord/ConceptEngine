#include "CEPhysicsScene.h"

#include "Physics/CEPhysicsMaterial.h"
#include "Rendering/Resources/Mesh.h"

//TODO: Implement...
void CEPhysicsScene::Release() {
}

//TODO: Implement...
void CEPhysicsScene::Simulate(float TS, bool CallFixedUpdate) {
}

//TODO: Implement...
void CEPhysicsScene::RemoveActor(CEPhysicsActor* PhysicsActor) {
}

//TODO: Implement...
bool CEPhysicsScene::Raycast(const XMFLOAT3& Origin, const XMFLOAT3& Direction, float MaxDistance,
                             CERayCastHit* OutHit) {
}

//TODO: Implement...
bool CEPhysicsScene::OverlapBox(const XMFLOAT3& Origin, const XMFLOAT3& HalfSize,
                                CEStaticArray<physx::PxOverlapHit, 10>& Buffer, uint32& Count) {
}

//TODO: Implement...
bool CEPhysicsScene::OverlapCapsule(const XMFLOAT3& Origin, float Radius, float HalfHeight,
                                    CEStaticArray<physx::PxOverlapHit, 10>& Buffer, uint32& Count) {
}

//TODO: Implement...
bool CEPhysicsScene::OverlapSphere(const XMFLOAT3& Origin, float Radius, CEStaticArray<physx::PxOverlapHit, 10>& Buffer,
                                   uint32& Count) {
}

//TODO: Implement...
void CEPhysicsScene::CreateRegions() {
}

//TODO: Implement...
bool CEPhysicsScene::Advance(CETimestamp TS) {
}

//TODO: Implement...
void CEPhysicsScene::SubStepStrategy(CETimestamp TS) {
}

//TODO: Implement...
bool CEPhysicsScene::OverlapGeometry(DirectX::XMFLOAT3& Origin, const physx::PxGeometry& Geometry,
                                     CEStaticArray<physx::PxOverlapHit, 10>& Buffer, uint32& Count) {
}

//TODO: Implement...
CEPhysicsActor* CEPhysicsScene::GetActor(Actor* InActor) {
	for (auto* Actor : Actors) {
		if (false /*TODO: Check if Actor already exists as PhysicsActor... (Check its settings) */) {
			return Actor;
		}
	}

	return nullptr;
}

//TODO: Implement
CEPhysicsActor* CEPhysicsScene::CreateActor(Actor* InActor) {
	return nullptr;
}
