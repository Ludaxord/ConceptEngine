#include "CEPhysicsScene.h"

#include "Boot/CECore.h"
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

void CEPhysicsScene::Release() {
	Assert(PhysXScene);

	for (auto& Actor : Actors)
		RemoveActor(Actor);

	Actors.Clear();
	PhysXScene->release();
	PhysXScene = nullptr;
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

bool CEPhysicsScene::Raycast(const XMFLOAT3& Origin,
                             const XMFLOAT3& Direction,
                             float MaxDistance,
                             CERayCastHit* OutHit) {
	physx::PxRaycastBuffer HitInfo;

	XMFLOAT3 NormalizeDirection;
	XMStoreFloat3(&NormalizeDirection, DirectX::XMVector3Normalize(XMLoadFloat3(&Direction)));

	bool Result = PhysXScene->raycast(ToPhysXVector(Origin), ToPhysXVector(NormalizeDirection), MaxDistance, HitInfo);
	if (Result) {
		CEPhysicsActor* Actor = (CEPhysicsActor*)HitInfo.block.actor->userData;
		OutHit->HitEntity = Actor->GetOwningActor()->GetUUID();
		OutHit->Position = FromPhysXVector(HitInfo.block.position);
		OutHit->Normal = FromPhysXVector(HitInfo.block.normal);
		OutHit->Distance = HitInfo.block.distance;
	}

	return Result;
}

bool CEPhysicsScene::OverlapBox(XMFLOAT3& Origin,
                                const XMFLOAT3& HalfSize,
                                CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
                                uint32& Count) {
	return OverlapGeometry(Origin, physx::PxBoxGeometry(HalfSize.x, HalfSize.y, HalfSize.z), Buffer, Count);
}

bool CEPhysicsScene::OverlapCapsule(XMFLOAT3& Origin,
                                    float Radius,
                                    float HalfHeight,
                                    CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
                                    uint32& Count) {
	return OverlapGeometry(Origin, physx::PxCapsuleGeometry(Radius, HalfHeight), Buffer, Count);
}

bool CEPhysicsScene::OverlapSphere(XMFLOAT3& Origin,
                                   float Radius,
                                   CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
                                   uint32& Count) {
	return OverlapGeometry(Origin, physx::PxSphereGeometry(Radius), Buffer, Count);
}

void CEPhysicsScene::CreateRegions() {
	const CEPhysicsConfig& Config = CECore::GetPhysics()->GetConfig();
	if (Config.BroadPhaseAlgorithm != PhysicsBroadPhaseType::AutomaticBoxPrune) {
		physx::PxBounds3* RegionBounds = new physx::PxBounds3[(size_t)Config.WorldBoundsSubdivisions * Config.
			WorldBoundsSubdivisions];
		physx::PxBounds3 GlobalBounds(ToPhysXVector(Config.WorldBoundsMin), ToPhysXVector(Config.WorldBoundsMax));
		uint32 RegionCount = physx::PxBroadPhaseExt::createRegionsFromWorldBounds(
			RegionBounds, GlobalBounds, Config.WorldBoundsSubdivisions);

		for (uint32 i = 0; i < RegionCount; i++) {
			physx::PxBroadPhaseRegion Region;
			Region.bounds = RegionBounds[i];
			PhysXScene->addBroadPhaseRegion(Region);
		}
	}
}

bool CEPhysicsScene::Advance(CETimestamp TS) {
	SubStepStrategy(TS);

	if (NumSubSteps == 0)
		return false;

	CE_LOG_DEBUG(
		"[CEPhysicsScene] Sub Steps: " + std::to_string(NumSubSteps) + " Sub Step Size: " + std::to_string(SubStepSize.
			AsSeconds()
		)
	);

	for (uint32 i = 0; i < NumSubSteps; i++) {
		PhysXScene->simulate(SubStepSize.AsNanoSeconds());
		PhysXScene->fetchResults(true);
	}

	return true;
}

void CEPhysicsScene::SubStepStrategy(CETimestamp TS) {
	if (Accumulator > SubStepSize)
		Accumulator = 0.0f;

	Accumulator += TS;
	if (Accumulator < SubStepSize) {
		NumSubSteps = 0;
		return;
	}

	NumSubSteps = Math::Min((uint32)(Accumulator / SubStepSize).AsNanoSeconds(), MaxSubSteps);

	Accumulator -= NumSubSteps * SubStepSize;

}

bool CEPhysicsScene::OverlapGeometry(DirectX::XMFLOAT3& Origin,
                                     const physx::PxGeometry& Geometry,
                                     CEStaticArray<physx::PxOverlapHit, OVERLAP_MAX_COLLISION>& Buffer,
                                     uint32& Count) {
	physx::PxOverlapBuffer Buf(Buffer.Data(), OVERLAP_MAX_COLLISION);

	XMFLOAT4X4 PoseTranslation;
	XMStoreFloat4x4(&PoseTranslation, DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&Origin)));
	physx::PxTransform Pose = ToPhysXTransform(PoseTranslation);

	const bool Result = PhysXScene->overlap(Geometry, Pose, Buf);

	if (Result) {
		memcpy(Buffer.Data(), Buf.touches, Buf.nbTouches * sizeof(physx::PxOverlapHit));
		Count = Buf.nbTouches;
	}

	return Result;
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
