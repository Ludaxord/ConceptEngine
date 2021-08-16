#pragma once
#include "Physics/CEPhysicsScene.h"

class CEPhysXScene : public CEPhysicsScene {
public:
	CEPhysXScene();
	void Release() override;
};
