#pragma once

class CEPhysicsScene {
public:
	CEPhysicsScene();
	virtual ~CEPhysicsScene();

	virtual void Release() = 0;
};
