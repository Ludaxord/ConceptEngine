#pragma once
#include "../Time/CETimestamp.h"

class CEPhysicsScene;

class CEPhysics {
public:
	virtual ~CEPhysics() = default;
	virtual bool Create() = 0;
	virtual bool CreateScene() = 0;
	virtual bool CreateConfig() = 0;
	virtual void Release() = 0;
	virtual void ReleaseScene() = 0;

	virtual void CreateActors(class Scene* Scene) = 0;
	virtual class CEPhysicsActor* CreateActor(class Actor* InActor) = 0;

	CEPhysicsScene* GetScene() {
		return PScene;
	}

	CEPhysicsConfig GetConfig() {
		return PhysicsConfig;
	}

protected:
	CEPhysicsConfig PhysicsConfig;
	CEPhysicsScene* PScene = nullptr;
};
