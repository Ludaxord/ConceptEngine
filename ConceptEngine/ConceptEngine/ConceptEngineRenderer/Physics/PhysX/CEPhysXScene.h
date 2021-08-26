#pragma once
#include "Physics/CEPhysicsScene.h"

class CEPhysXScene : public CEPhysicsScene {
public:
	CEPhysXScene(const CEPhysicsConfig& Config);
	void Release() override;
};
