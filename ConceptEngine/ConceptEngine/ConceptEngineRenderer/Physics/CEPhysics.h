#pragma once
#include "../Time/CETimestamp.h"

class CEPhysics {
public:
	virtual bool Create() = 0;
	virtual bool CreateScene() = 0;
	virtual void Release() = 0;
	virtual void ReleaseScene() = 0;

	virtual void CreateActors(class Scene* Scene) = 0;
	virtual void CreateActor();

protected:
	CEPhysicsConfig PhysicsConfig;
	class CEPhysicsScene* PScene = nullptr;
};
