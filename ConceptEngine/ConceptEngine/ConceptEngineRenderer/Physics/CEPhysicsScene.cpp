#include "CEPhysicsScene.h"

#include "Physics/CEPhysicsMaterial.h"
#include "PhysX/CEContactCallback.h"
#include "PhysX/CEPhysX.h"
#include "Rendering/Resources/Mesh.h"
#include "Scene/Components/CEIDComponent.h"

static CEContactCallback ContactCallback;

CEPhysicsScene::CEPhysicsScene(const CEPhysicsConfig& Config) : SubStepSize(Config.FixedTimeStep) {
	physx::PxSceneDesc SceneDesc(CEPhysX::GetPhysXSDK().getTolerancesScale());

#if ENABLE_ACTIVE_ACTORS
	SceneDesc.flags |=  physx::PxSceneFlag::eENABLE_CCD | physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;
#else
	SceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
#endif

	SceneDesc.gravity = ToPhysXVector(Config.Gravity);
	SceneDesc.broadPhaseType = CEPhysX::GetPhysXBroadPhaseType(Config.BroadPhaseAlgorithm);
	SceneDesc.cpuDispatcher = &CEPhysX::GetCPUDispatcher();
	SceneDesc.filterShader = (physx::PxSimulationFilterShader)CEPhysX::FilterShader;
	SceneDesc.simulationEventCallback = &ContactCallback;
	SceneDesc.frictionType = CEPhysX::GetPhysXFrictionType(Config.FrctionModel);

	Assert(SceneDesc.isValid());

	PhysXScene = CEPhysX::GetPhysXSDK().createScene(SceneDesc);
	Assert(PhysXScene);
}

CEPhysicsScene::~CEPhysicsScene() {
}

//TODO: Implement...
void CEPhysicsScene::Release() {
}

void CEPhysicsScene::Simulate(CETimestamp TS, bool CallFixedUpdate) {
	if (CallFixedUpdate) {
		for (auto& Actor : Actors)
			Actor->OnFixedUpdate(SubStepSize);
	}

	if (Advance(TS)) {
#if ENABLE_ACTIVE_ACTORS
		uint32 ActiveActorsNumber;
		physx::PxActor** ActiveActors = PhysXScene->getActiveActors(ActiveActorsNumber);
		for (uint32 i = 0; i < ActiveActorsNumber; i++) {
			CEPhysicsActor* Actor = (CEPhysicsActor*)ActiveActors[i]->userData;
			Actor->SyncTransform();
		}
#else
		for (auto& Actor : Actors)
			Actor->SyncTransform();
#endif
	}
}

//TODO: Implement...
void CEPhysicsScene::RemoveActor(CEPhysicsActor* PhysicsActor) {
	if (!PhysicsActor)
		return;

	for (auto& Collider : PhysicsActor->Colliders) {
		Collider->DetachFromActor(PhysicsActor->RigidActor);
		Collider->Release();
	}

	PhysXScene->removeActor(*PhysicsActor->RigidActor);
	PhysicsActor->RigidActor->release();
	PhysicsActor->RigidActor = nullptr;

	for (auto A = Actors.Begin(); A != Actors.End(); A++) {
		if ((*A)->GetOwningActor()->GetUUID() == PhysicsActor->GetOwningActor()->GetUUID()) {
			Actors.Erase(A);
			break;
		}
	}
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

CEPhysicsActor* CEPhysicsScene::GetActor(Actor* InActor) {
	for (auto* Actor : Actors) {
		if (Actor->GetUUID() == InActor->GetUUID()) {
			return Actor;
		}
	}

	return nullptr;
}

CEPhysicsActor* CEPhysicsScene::CreateActor(Actor* InActor) {
	CEPhysicsActor* Actor = DBG_NEW CEPhysicsActor(InActor);
	Actors.PushBack(Actor);
	PhysXScene->addActor(*Actor->RigidActor);
	return Actor;
}
