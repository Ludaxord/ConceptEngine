#pragma once
#include "Time/CETimestamp.h"

class CEPhysics {
public:
	virtual bool Create() = 0;
	virtual bool CreateScene() = 0;
	virtual void Release() = 0;

	virtual void Update(CETimestamp DeltaTime) = 0;

	virtual std::string CreatePXRigidStatic(void* Desc) = 0;
	virtual std::string CreatePXRigidDynamic(void* Desc) = 0;
};
