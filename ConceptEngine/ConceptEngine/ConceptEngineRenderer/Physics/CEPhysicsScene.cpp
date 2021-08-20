#include "CEPhysicsScene.h"

CEPhysicsActor* CEPhysicsScene::GetActor(Actor* Actor) {
	for (auto* Actor : Actors) {
		if (false /*TODO: Check if Actor already exists as PhysicsActor... (Check its settings) */) {
			return Actor;
		}
	}

	return nullptr;
}

CEPhysicsActor* CEPhysicsScene::CreateActor(Actor* Actor) {
	return nullptr;
}
