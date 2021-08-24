#include "CEContactCallback.h"

#include "Physics/CEPhysicsActor.h"
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

	CEPhysicsActor* ActorA;
	CEPhysicsActor* ActorB;

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

//TODO: Implement
void CEContactCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {
}

//TODO: Implement
void CEContactCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer,
                                  const physx::PxU32 count) {
}
