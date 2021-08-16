#pragma once

class CEPhysicsScene {
public:
	CEPhysicsScene() {
	};
	virtual ~CEPhysicsScene() = default;

	virtual void Release() = 0;
};
