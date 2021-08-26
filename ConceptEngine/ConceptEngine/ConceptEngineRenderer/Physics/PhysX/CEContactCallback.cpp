#include "CEContactCallback.h"

#include "Physics/CEPhysicsActor.h"
#include "Physics/CEPhysicsMaterial.h"
#include "Rendering/Resources/Mesh.h"
#include "Scene/CEScene.h"
#include "Scene/Components/CETagComponent.h"

void CEContactCallback::onConstraintBreak(physx::PxConstraintInfo* Constraints, physx::PxU32 Count) {
	PX_UNUSED(Constraints);
	PX_UNUSED(Count);
}

void CEContactCallback::onWake(physx::PxActor** Actors, physx::PxU32 Count) {
	for (uint32 i = 0; i < Count; i++) {
		physx::PxActor& PhysXActor = *Actors[i];
		CEPhysicsActor* Actor = (CEPhysicsActor*)PhysXActor.userData;
		CE_LOG_INFO(
			"[CEContactCallback]: PhysX Actor Waking Up -> ID: " + std::to_string(Actor->GetOwningActor()->GetUUID()) +
			" Name: " + Actor->GetOwningActor()->GetComponentOfType<CETagComponent>()->Tag
		);
	}
}

void CEContactCallback::onSleep(physx::PxActor** Actors, physx::PxU32 Count) {
	for (uint32 i = 0; i < Count; i++) {
		physx::PxActor& PhysXActor = *Actors[i];
		CEPhysicsActor* Actor = (CEPhysicsActor*)PhysXActor.userData;
		CE_LOG_INFO(
			"[CEContactCallback]: PhysX Actor Sleep -> ID: " + std::to_string(Actor->GetOwningActor()->GetUUID()) +
			" Name: " + Actor->GetOwningActor()->GetComponentOfType<CETagComponent>()->Tag
		);
	}
}

void CEContactCallback::onContact(const physx::PxContactPairHeader& PairHeader,
                                  const physx::PxContactPair* Pairs,
                                  physx::PxU32 nbPairs) {
	auto RemovedActorA = PairHeader.flags & physx::PxContactPairHeaderFlag::eREMOVED_ACTOR_0;
	auto RemovedActorB = PairHeader.flags & physx::PxContactPairHeaderFlag::eREMOVED_ACTOR_1;

	CEPhysicsActor* ActorA = nullptr;
	CEPhysicsActor* ActorB = nullptr;

	if (!RemovedActorA)
		ActorA = (CEPhysicsActor*)PairHeader.actors[0]->userData;

	if (!RemovedActorB)
		ActorB = (CEPhysicsActor*)PairHeader.actors[1]->userData;

	CEPhysicsActor* ValidActor = ActorA ? ActorA : ActorB;
	if (!ValidActor || !CEScene::GetSceneByUUID(ValidActor->GetOwningActor()->GetSceneUUID())->IsPlaying())
		return;

	if (Pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH) {
		//TODO: Create Scripting Engine...
	}
	else if (Pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH) {
		//TODO: Create Scripting Engine...
	}
}

void CEContactCallback::onTrigger(physx::PxTriggerPair* Pairs, physx::PxU32 Count) {
	auto RemovedTrigger = Pairs->flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER;
	auto RemovedOther = Pairs->flags & physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER;

	CEPhysicsActor* TriggerActor = nullptr;
	CEPhysicsActor* OtherActor = nullptr;
	if (!RemovedTrigger)
		TriggerActor = (CEPhysicsActor*)Pairs->triggerActor->userData;

	if (!RemovedOther)
		OtherActor = (CEPhysicsActor*)Pairs->otherActor->userData;

	CEPhysicsActor* ValidActor = TriggerActor ? TriggerActor : OtherActor;
	if (!ValidActor || !CEScene::GetSceneByUUID(ValidActor->GetOwningActor()->GetSceneUUID())->IsPlaying())
		return;

	if (Pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
		//TODO: Create Scripting Engine...
	}
	else if (Pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
		//TODO: Create Scripting Engine...
	}
}

void CEContactCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer,
                                  const physx::PxU32 count) {
	PX_UNUSED(bodyBuffer);
	PX_UNUSED(poseBuffer);
	PX_UNUSED(count);
}
