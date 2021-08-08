#pragma once
#include "CERigid.h"

class CERigidStatic : public CERigid {
public:
	CERigidStatic(std::string& InParentName, const CERigidTransform& InParentTransform,
	              const CERigidTransform& InLocalTransform);
	~CERigidStatic();
	void AddRigidStatic() override;
	void Release() override;
	void Create() override;

	void CreatePhysicsMesh();
	void CreatePhysicsComponent();
};
